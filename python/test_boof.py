import numpy as np
import pyboof as pb
import cv2
import os
import time


video_path = '~/Downloads/'

# VideoCapture
cap = cv2.VideoCapture(0, cv2.CAP_V4L2)
cap.set(3, 1280)
cap.set(4, 720)
cap.set(5, 30)

# VideoWriter
fourcc = cv2.VideoWriter_fourcc(*'mp4v')
outVideo = cv2.VideoWriter()
outVideo.open('output.mp4',fourcc,10.0,(1280,720), True )

# QRCodeDetector
findQR = False
qrResult = ''
pb.init_memmap()
detector = pb.FactoryFiducial(np.uint8).qrcode()



print('Demo will work')
cnt = 0

while(cap.isOpened()):
    ret, frame = cap.read()
    if ret==True:
        #frame = cv2.flip(frame,-1)
        outVideo.write(frame)
        
        # QR Code Detector
        if not findQR :
            frame_mono = cv2.cvtColor(np.uint8(frame), cv2.COLOR_BGR2GRAY)
            boof_img = pb.ndarray_to_boof(frame_mono)
            #boof_img = pb.load_single_band( 'path to image', np.uint8)
            start = time.time_ns()
            detector.detect(boof_img)
            if len(detector.detections)>0:
                end = time.time_ns()
                print("running time is ", str(end - start))
                print("find QR Code " )
                findQR = True
                qrResult = detector.detections[0].message

    else:
        print('fail to open camera')
        break
    cnt += 1
    if cnt > 100 :
        print('Have created output.mp4')
        break

if findQR:
    print("QR Code  is: {}".format(qrResult))
else:
    print("Didn't find QR Code!")

cap.release()
outVideo.release()
cv2.destroyAllWindows()


    
