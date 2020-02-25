import numpy as np
import cv2
import os

video_path = '~/Downloads/'


cap = cv2.VideoCapture(0, cv2.CAP_V4L2)
cap.set(3, 1280)
cap.set(4, 720)
cap.set(5, 30)
fourcc = cv2.VideoWriter_fourcc(*'mp4v')
outVideo = cv2.VideoWriter()
outVideo.open('output.mp4',fourcc,30.0,(1280,720), True )

print('Demo will work')
cnt = 0

while(cap.isOpened()):
    ret, frame = cap.read()
    if ret==True:
        frame = cv2.flip(frame,-1)
        outVideo.write(frame)
    else:
        print('fail to open camera')
        break
    cnt += 1
    if cnt > 900 :
        print('Have created output.avi')
        break

cap.release()
outVideo.release()
cv2.destroyAllWindows()


    
