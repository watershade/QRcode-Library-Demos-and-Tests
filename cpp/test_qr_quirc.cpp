/**
 * @file: test_qr.cpp
 * @brief:capture video from camera and writer into disk by date
 * @author: zjh
 * date Jan 20， 2020
*/

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <stdio.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <time.h>
#include <quirc.h>
//#include <ctime>

using namespace cv;
using namespace std;
const string outFile = "./out.avi";
const int MaxNumOfFrames = 900;
std::string qr_result;
volatile bool findQR = false;
volatile bool isNewImg = false;
volatile bool letTaskQRExit = false;
cv::Mat shadow;
struct quirc *qrReader;

mutex mtxShadow;
condition_variable cvImgOut;

/*camera size and other parameter config*/
const int CAMIMG_WIDTH = 1280;
const int CAMIMG_HEIGHT = 720;
const int CAMIMG_FPS = 30;
const int QUIRC_MEM_SIZE = (CAMIMG_WIDTH*CAMIMG_HEIGHT)/4;

void scanner(int &frameId)
{
	std::string temp;
	cv::Mat gray_img_qtr;


	int fNum = 0;
	clock_t sumColck = 0;
	findQR = false;
	//cv::Mat grayShadow;
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
		//cout<< "get a new img for QR" << endl;
		//cout<<"fID:" << frameId <<endl;
		clock_t start = clock();
		
		//int grayImgW, grayImgH;
		//uint8_t *qr_img = quirc_begin(qrReader, &grayImgW, &grayImgH);
		uint8_t *qr_img = quirc_begin(qrReader, NULL, NULL);
		resize( shadow, gray_img_qtr, Size(CAMIMG_WIDTH/2, CAMIMG_HEIGHT/2), 0, 0, INTER_CUBIC );
		//resize( shadow, gray_img_qtr, Size(CAMIMG_WIDTH/2, CAMIMG_HEIGHT/2) );
		//shadow.convertTo(gray_uchar_img, CV_8U);
		memcpy(qr_img, gray_img_qtr.data, QUIRC_MEM_SIZE);
		quirc_end(qrReader);

		sumColck += (clock() - start);
		fNum++;

		isNewImg = false;

		if (quirc_count(qrReader) > 0)
		{
			/*retrive qrcode from result*/
			struct quirc_code qr_code_raw;
			struct quirc_data temp;

			/*no matter how many qrCode, just retrive one*/
			quirc_extract(qrReader, 0, &qr_code_raw);
			if(quirc_decode(&qr_code_raw, &temp) ){
				/*decode error ,can not find useful QR code and continue*/
				cerr << "Err/quirc: decode quirc_code error" << endl;
				continue; 
			}
			/*succeed*/
			
			
			/*set result*/
			findQR = true;
			std::cout << "Have find QR" << endl;
			std::cout << "CV detect period is about " << sumColck / fNum << "clocks" << endl;
			
			qr_result.assign((char*)(temp.payload));
			return;
		}
		// else
		// {
		// 	findQR = false;
		// }
	}
}

int main(int argc, char **argv)
{
	cv::Mat frame;
	int numOfFrame = 0;

	/*initial quirc*/
	qrReader = quirc_new();
	if (!qrReader) {
        cerr<< "Err/quirc: Failed to allocate memory"<< endl;
		return -1;
	}
	if (quirc_resize(qrReader, CAMIMG_WIDTH, CAMIMG_HEIGHT) < 0)
	{
		cerr<< "Err/quirc: Failed to allocate video memory"<< endl;
		return -1;
	}

	//Using VideoCapture to get images from camera
	//camera is in /dev/video0
	VideoCapture cap;
	cap.set(cv::CAP_PROP_FRAME_WIDTH, CAMIMG_WIDTH);
	cap.set(cv::CAP_PROP_FRAME_HEIGHT, CAMIMG_HEIGHT);
	cap.set(cv::CAP_PROP_FPS, CAMIMG_FPS);
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
			cv::cvtColor(frame, shadow, cv::COLOR_BGR2GRAY );
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

	/*release VideoCapture*/
	if (cap.isOpened())
	{
		cout << "Finish capturing and close later" << endl;
		cap.release();
	}
	/* destroy quirc and release memory*/
	quirc_destroy(qrReader);

	/*wait subthread exit and join it*/
	if (taskQR.joinable())
		taskQR.join();
	
	/*show the qrcode result*/
	if (findQR)
		cout << "QR code is:\r\n\t" << qr_result << endl;

	return 0;
}
