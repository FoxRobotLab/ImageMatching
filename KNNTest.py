
import numpy as np
import cv2
import cv2.ml as cv2ml



def addInformation(imNum, rowData, first, last, label):
    numInCat = last - first + 1
    trainNum = int(numInCat * 0.8)
    testNum = numInCat - trainNum
    if imNum < (first + trainNum):
        addToTrain(rowData, label)
    else:
        addToTest(rowData, label)


def addToTrain(data, label):
    global trainData
    global trainLabels
    if trainData == None:
        trainData = data
    else:
        trainData = np.vstack( (trainData, data) )
    trainLabels.append(label)
    
def addToTest(data, label):
    global testData
    global testLabels
    if testData == None:
        testData = data
    else:
        testData = np.vstack( (testData, data) )
    testLabels.append(label)
    
        
picDir = "Pictures2016/May23-MobileTest4/"
formStr = "{0:s}pic{1:0>4d}.{2:s}"

firstNorth = 0
lastNorth = 48
firstEast = lastNorth + 1
lastEast = 85
#firstSouth = lastEast + 1
#lastSouth = 131
#firstWest = lastSouth + 1
#lastWest = 174


north = 0
east = 1
south = 2
west = 3

trainData = None
testData = None
trainLabels = []
testLabels = []

for i in range(86):
    name = formStr.format(picDir, i, "jpg")
    nextIm = cv2.imread(name)
    (h, w, d) = nextIm.shape
    nextRow = nextIm.reshape( (1, h * w * d) ).astype(np.float32)
    if i <= lastNorth:
        addInformation(i, nextRow, firstNorth, lastNorth, north)
    elif i <= lastEast:
        addInformation(i, nextRow, firstEast, lastEast, east)
    elif i <= lastSouth:
        addInformation(i, nextRow, firstSouth, lastSouth, south)
    elif i <= lastWest:
        addInformation(i, nextRow, firstWest, lastWest, west)
        


trainLabels = np.array(trainLabels).astype(np.float32)
testLabels = np.array(testLabels).astype(np.float32)

knn = cv2ml.KNearest_create()
knn.train(trainData, cv2ml.ROW_SAMPLE, trainLabels)
ret, results, neighbours ,dist = knn.findNearest(testData, 3)

print testLabels
print results.ravel()
print len(testData)
print results.shape



print "result: ", results,"\n"
print "neighbours: ", neighbours,"\n"
print "distance: ", dist

