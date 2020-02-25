/**
 * @file: test_cam.cpp
 * @brief:capture video from camera and writer into disk by date
 * @author: zjh
 * date Jan 16， 2020
*/

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
//#include <opencv2/objdetect.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <stdio.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <zbar.h>
#include <time.h>
// #include <zbar/Decoder.h>
// #include <zbar/ImageScanner.h>
// #include <zbar/Image.h>
// #include <zbar/Symbol.h>
//#include <ctime>

using namespace cv;
using namespace std;
using namespace zbar;
const string outFile = "./out.avi";
const int MaxNumOfFrames = 900;
std::string qr_result;
volatile bool findQR = false;
volatile bool isNewImg = false;
volatile bool letTaskQRExit = false;
cv::Mat shadow;

mutex mtxShadow;
condition_variable cvImgOut;

const int CAMIMG_WIDTH = 1280;
const int CAMIMG_HEIGHT = 720;

void scanner(int& frameId)
{
	std::string temp;
	//cv::QRCodeDetector reader = QRCodeDetector();
	zbar::ImageScanner scanner;
	scanner.set_config(ZBAR_QRCODE, ZBAR_CFG_ENABLE ,1);
	int fNum = 0;
	clock_t sumColck = 0;

	while ((!findQR) && (!letTaskQRExit))
	{
		unique_lock<mutex> lck(mtxShadow);
		while (!isNewImg)
			cvImgOut.wait(lck, [&] { return isNewImg; });
		if (shadow.empty())
		{
			cerr << "Use a blank frame to detect qr" << endl;
			isNewImg = false;
			break;
		}
		//cout<< "frameId:"<< frameId << endl;
		clock_t start = clock();
		int width = shadow.cols;
		int height = shadow.rows;
		zbar::Image targetImg(width, height,"Y800", shadow.data, width*height);
		int retNum = scanner.scan(targetImg);
		sumColck += (clock() - start);
		fNum++;
		isNewImg = false;

		if (retNum > 0)
		{
			std::string temp = scanner.get_results().symbol_begin()->get_data();
			findQR = true;
			std::cout << "Have find QR" << endl;
			std::cout << "zbar detect period is about " << sumColck/fNum << "clocks"<<endl;
			std::cout << " there are " << CLOCKS_PER_SEC << " in a seconds" <<endl;
			qr_result.assign(temp);
			return;
		}
		else
		{
			findQR = false;
		}
	}
}

int main(int argc, char **argv)
{
	cv::Mat frame;
	int numOfFrame = 0;

	//Using VideoCapture to get images from camera
	//camera is in /dev/video0
	VideoCapture cap;
	cap.set(cv::CAP_PROP_FRAME_WIDTH, CAMIMG_WIDTH);
	cap.set(cv::CAP_PROP_FRAME_HEIGHT, CAMIMG_HEIGHT);
	cap.set(cv::CAP_PROP_FPS, 30);
	cap.set(cv::CAP_PROP_AUTOFOCUS, 0);

	//try to open  VideoCapture
	cap.open("/dev/video1", cv::CAP_V4L2);
	if (!cap.isOpened())
	{
		cerr << "ERROR! Unable to open camera\r\n";
		return -1;
	}

	cap >> frame;
	if (frame.empty())
	{
		cerr << "ERROR! blank frame grabbed\r\n";
		return -1;
	}
	bool isColor = (frame.type() == CV_8UC3);

	// Using VideoWriter to storage the image stream from camera
	VideoWriter writer;
	//int codec = VideoWriter::fourcc('M', 'J', 'P', 'G');
	int codec = VideoWriter::fourcc('D', 'I', 'V', 'X');
	writer.open(outFile, codec, 30.0, frame.size(), isColor);
	if (!writer.isOpened())
	{
		cerr << "Couldn't open the output video file for write\r\n";
		return -1;
	}
	
	cout << "We will grab " << MaxNumOfFrames << " frames and Storage it in " << outFile << endl;
	cout << "Start to write frames\r\n"
		 << endl;
	//clock_t start = clock();

	letTaskQRExit = false;
	findQR = false;
	thread taskQR(scanner, std::ref(numOfFrame));

	int64 start = getTickCount();

	while (numOfFrame < MaxNumOfFrames)
	{
		if (!cap.read(frame))
		{
			cerr << "ERROR! blank frame #" << numOfFrame << " grabbed\r\n";
			break;
		}
		//flip(frame,frame,1);

		numOfFrame++;
		if (numOfFrame >= MaxNumOfFrames)
		{
			letTaskQRExit = true;
		}

		if ((!isNewImg) || letTaskQRExit)
		{
			unique_lock<mutex> lck(mtxShadow, std::defer_lock);
			lck.lock();
			//frame.copyTo(shadow);
			cv::cvtColor( frame, shadow, cv::COLOR_BGR2GRAY );
			isNewImg = true;
			lck.unlock();
			cvImgOut.notify_one();
		}

		writer.write(frame);
	}
	//clock_t end = clock();
	int64 interval = getTickCount() - start;
	//cout << "Grab "<< MaxNumOfFrames << " frames need "<< (end -start)/(CLOCKS_PER_SEC/1000) <<" milliseconds" << endl;
	cout << "Grab " << MaxNumOfFrames << " frames need " << (interval / getTickFrequency()) << " milliseconds" << endl;

	if (cap.isOpened())
	{
		cout << "Finish capturing and close later" << endl;
		cap.release();
	}

	if (taskQR.joinable())
		taskQR.join();

	if (findQR)
		cout << "QR code is:\r\n\t" << qr_result << endl;

	return 0;
}
