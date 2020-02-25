# qrcode-scanner 
## Overview
This is a project to test different qrcode library. All of this file have functions like this:
  >.1 Open Webcam , capture frames from it and storage them on disk. 
  >.2 Try to find a QRCode and recognize from it. Some library will find many qrcode but we just use one of them.

There are three folders: python ,CPP and others. Each of them  have different programs and named in library. Others is nor program but the test result.
All of them have been test on Armbian Debian os. And the hardware is orange pi 3 using Allwinner H6 CPU and 2G DDR3.   
All camera capture frame in size:1280,720. 

## Python
 .1 test_cam.py : It is a basic program to test video. And we use the num 0 camera. And you can change it easily.
 .2 test_qr.py : Using QRCodeDetector in opencv4 to detect QRCode.
 .3 test_boof.py : Using BoofCV(pyboof) to detect QRCode.
 .4 test_zbar.py : Using zbar to detect QRcode. 
 .5 test_zxing.py : Using zxing to detect QRcode.
## CPP
 .1 test_qr.cpp : It is a program to recoder webcam using VideoCapture and VideoWriter. Using QRCodeDetector to detect QRCode.
 .2 test_qr_zbar.cpp : It is just like testqr.cpp but use zbar library to detect QRcode instead.
 .3 test_qr_quirc.cpp : It is just like testqr.cpp but use quirc library to detect QRcode instead.Cause some memmory limited, we resize (1280,720) source image to (640,480) target image. And it worked well.

 ## others
 .1 opencv.pc : the pkgconfig file for opencv4. If you need build file in CPP, you may need install opencv4 and config pkg-config for opencv4.
 .2 zbar file : the pkgconfig file for zbar. You may need it when you build test_qr_zbar.cpp
 .3 quirc file : the pkgconfig file for quirc. You may need it when you build test_qr_quirc.cpp
