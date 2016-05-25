
import numpy as np
import cv2
import time

def makePictureLog(picDir, startNum):
    
    camera = cv2.VideoCapture(0)
    cv2.namedWindow("Live Feed")
    cv2.namedWindow("Saved Picture")
    cv2.moveWindow("Live Feed", 0, 0)
    cv2.moveWindow("Saved Picture", 0, 500)
    badCount = 0
    prevTime = time.time()
    picNum = startNum
    while True:
        res, frame = camera.read()
        if not res:
            print "ERROR: couldn't read frame"
            badCount += 1
            if badCount > 100:
                print "... Quitting, camera failure"
                break
        else:
            badCount = 0
            feedFrame = cv2.resize(frame, dsize = (0, 0), fx = 0.5, fy = 0.5)
            cv2.imshow("Live Feed", feedFrame)
            currTime = time.time()
            if (currTime - prevTime) > 1.10:
                stillPic = feedFrame
                #cv2.imshow("Saved Picture", stillPic)
                filename = makeFilename(picDir, picNum)
                ret = cv2.imwrite(filename, stillPic)
                if not ret:
                    print "PICTURE DIDN'T SAVE"
                picNum += 1
                prevTime = currTime
            
            x = cv2.waitKey(30)
            ch = chr(x & 0xFF)
            if ch == 'q':
                break
    cv2.destroyAllWindows()
    
    
def makeFilename(directory, fileNum, extension = 'jpg'):
    """Makes a filename for reading or writing image files"""
    formStr = "{0:s}pic{1:0>4d}.{2:s}"
    name = formStr.format(directory, fileNum, extension)
    return name
  
  
  
  
makePictureLog("Pictures2016/May23-MobileTest4/", 0)

