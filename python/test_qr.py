import numpy as np
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
outVideo.open('output.mp4',fourcc,30.0,(1280,720), True )

# QRCodeDetector
findQR = False
qrResult = ''
qrDetector = cv2.QRCodeDetector()




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
            start = time.time_ns()
            qrDone, points = qrDetector.detect(frame_mono)
            if qrDone:
                #print("find QR Code. Frame counter is ", cnt )
                qrString, straight = qrDetector.decode(frame_mono, points)
                if len(qrString) >1:
                    end = time.time_ns()
                    print("Running time is ",str(end - start))
                    findQR = True
                    qrResult = qrString
                    print("find QR Code")

    else:
        print('fail to open camera')
        break
    cnt += 1
    if cnt > 300 :
        print('Have created output.avi')
        break

if findQR:
    print("QR Code  is: {}".format(qrResult))
else:
    print("Didn't find QR Code!")

cap.release()
outVideo.release()
cv2.destroyAllWindows()


    
