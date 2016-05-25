""" ========================================================================
 * HoughLines.py
 *
 *  Created on: May 2016
 *  Author: susan
 *
 *  The HoughLines object contains information about lines found in the image using the cvCanny edge detection and
 *  Hough lines detection algorithms.  It copies the lines from the CvSeq format they are found in to a vector
 *  of vectors of CvPoints, for easier use.  It compares similarity between HoughLines objects by matching lines
 *  to each other, trying to maximize the similarity of matched lines.
 *
========================================================================="""


import math
import random
import time
import cv2
import numpy as np
import partitionAlg
import FeatureType
import FoxQueue
import OutputLogger


class HoughLines(FeatureType.FeatureType):
    """Holds data about Hough Lines found in the input picture."""
    def __init__(self, image, logger):
        """Takes in an image and a logger and builds the hough lines. Also
        initializes other instance variables that are needed"""
	FeatureType.FeatureType.__init__(self, image, logger, 120.0, 200.0)
        self.lines = []
        self._findHoughLines()



    def _findHoughLines(self):
	"""Finds canny edges and hough lines for the given image, initializing the
	lines instance variable to have the resulting endpoints of the lines."""
	img = self.image.copy()
	#cv2.imshow("HoughCopy", img)
	#cv2.moveWindow("HoughCopy", 400, 0)
	cannyImage = cv2.cvtColor(self.image, cv2.COLOR_BGR2GRAY)
	cannyImage = cv2.GaussianBlur(cannyImage, (5, 5), sigmaX = 0, sigmaY = 0)
	cannyImage = cv2.Canny(cannyImage, 10, 150, 3)
	kern1 = cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (5, 5))
	kern2 = cv2.getStructuringElement(cv2.MORPH_RECT, (3, 3))
	cannyImage = cv2.morphologyEx(cannyImage, cv2.MORPH_CLOSE, kern1)
	cannyImage = cv2.dilate(cannyImage, kern2)
	#cv2.imshow("Hough1 Canny", cannyImage)
	linesFound = cv2.HoughLinesP(cannyImage, rho = 1, 
                                     theta = np.pi/180, threshold=100,
                                     minLineLength=20, maxLineGap = 10)

	if linesFound == None:
	    self.lines = np.empty( (0, 5), dtype = np.int32)
	    return
	lines = linesFound.reshape( (-1, 4))
	dists = np.sqrt(np.sum( (lines[:,0:2] - lines[:,2:4]) ** 2, axis = 1))
	self.lines = lines[dists >= 20]
	
	#self.lines2 = np.empty( (0, 4), dtype = np.int32)
	#if linesFound == None:
	    #return
	#for i in range(len(linesFound)):
	    #currLine = linesFound[i][0]
	    #pt1 = currLine[0:2]
	    #pt2 = currLine[2:4]
	    #dist = np.sqrt(np.sum( (pt1 - pt2)**2))
	    #if dist >= 20:
		#newLine = currLine.reshape( (1, 4) )
		#self.lines2 = np.vstack( (self.lines2, newLine) )

	#print "Quickie"
	#print self.lines
	#print "Longie"
	#print self.lines2
	#raw_input("...")
	# Next, keep only representative lines and merge clusters of lines
	self.partitionLines()
	self._sortEndpoints()
	
	
    def _sortEndpoints(self):
	"""Sorts the points in lines so that they are ordered by their leftmost\
	point. Helper for findHoughLines"""
	for i in range(len(self.lines)):
	    [x1, y1, x2, y2, a] = self.lines[i]
	    if (x2 < x1) or ( (x2 == x1) and (y2 < y1) ):
		firstPt = self.lines[i,0:2].copy()
		self.lines[i,0:2] = self.lines[i,2:4]
		self.lines[i,2:4] = firstPt

			
	 
 
    def displayFeaturePics(self, windowName, startX, startY):
        """Given a window name and a starting location on the screen, this creates
        an image that represents the color signature and displays it."""
	#print "Number of lines:", len(self.lines)
        displayPic = self.image.copy()
        for i in range(len(self.lines)):
            [x1, y1, x2, y2, a] = self.lines[i]
	    colr = ( random.randint(0, 255), random.randint(0, 255), random.randint(0, 255) )
            cv2.line(displayPic, (x1, y1), (x2, y2), colr,2)
	cv2.imshow(windowName, displayPic)
	cv2.moveWindow(windowName, startX, startY)
    
    


    def evaluateSimilarity(self, otherHL):
        """Given another HoughLines object, evaluate the similarity. 
        Starts by making a priority queue for each line here.
        It then adds each line in the otherLines to queues for the lines that match.
        It then uses a greedy algorithm to pick matches (not optimal, but efficient).
        """
	numLines = len(self.lines)
	otherLines = otherHL.lines
	numOtherLines = len(otherLines)
	if abs(numLines - numOtherLines) > 30:
	    return 100.0
        matchQueues = [None] * numLines
        for thisPos in range(numLines):
            matchQueues[thisPos] = FoxQueue.PriorityQueue()
            for thatPos in range(numOtherLines):
                nextScore = self._compareLines(self.lines[thisPos], otherLines[thatPos])
                if nextScore >= 0.0:
                    matchQueues[thisPos].insert(thatPos, nextScore)
           	
	finalMatches = self._chooseFinalMatches(matchQueues)
	
		
	matchCount = 0
	totalScore = 0.0
	for i in range(numLines):
	    if finalMatches[i] != None:
		matchCount += 2
		(other, score) = finalMatches[i]
		totalScore += 2 * score
		
	unmatched = (numLines + numOtherLines) - matchCount
	penalty = 200 * unmatched
	aveScore = (totalScore + penalty) / (numLines + numOtherLines)
	#self.logger.log("Evaluate similarity, result is:" + str(aveScore))
	
	return self._normalizeSimValue(aveScore)
    
    

    def _compareLines(self, lineA, lineB):
        """A helper for evaluateSimilarity, it takes two lines and computes
        their similarity"""
	if len(lineA) == 5:
	    lineA = lineA[0:4]
	if len(lineB) == 5:
	    lineB = lineB[0:4]
	totalOne = self._euclideanDist(lineA, lineB)
	lineBFlip = np.array( [lineB[2], lineB[3], lineB[0], lineB[1]] )
	totalTwo = self._euclideanDist(lineA, lineBFlip)
        endPtDiff = min(totalOne, totalTwo)
        if endPtDiff > 100:  # if too different already, no match!
            return -1.0
        
        angleA = self._calcAngle(lineA)
        angleB = self._calcAngle(lineB)
        angleDiff = abs(angleA - angleB)
        if angleDiff > 30: # if too different then no match!
            return -1.0
	
        return endPtDiff + angleDiff
       

    def _euclideanDist(self, points1, points2):
	"""Computes Euclidean distance between two vectors, may have 2, 3, or 4 values in each one."""
	dist = np.sqrt( np.sum( (points1 - points2) ** 2 ) )
	return dist



    def _calcAllAngles(self, lineVec):
	"""Given a matrix where each row represents a line, compute all angles..."""
	xDiff = lineVec[:,2] - lineVec[:,0]           # points are sorted by x value
	yDiff = lineVec[:,1] - lineVec[:,3]           # top of screen is zero
	angle = np.arctan2(yDiff, xDiff) * (180 / np.pi)
	# angle	is  in the range [-90.0,90). xDiff can never be negative by 
	# sorting of points but yDiff may be. When yDiff is negative, the 
	# slope is negative, angle is in 2nd quadrant. We want to convert 
	# angle to be the range from [0.0, 180) by converting the negative 
	# angles to be positive, 2nd-quadrant angles
	angle[yDiff < 0] = angle[yDiff < 0] + 180
	return angle


	
    def _calcAngle(self, line):
        """Given a line, it computes the angle of the line in degrees, in the
        range [0, 180) where 0 is horizontal."""
        xDiff = line[2] - line[0]           # points are sorted by x value
        yDiff = line[1] - line[3]           # top of screen is zero
        angle = np.arctan2(yDiff, xDiff) * (180 / np.pi)
	# angle	is  in the range [-90.0,90). xDiff can never be negative by 
	# sorting of points but yDiff may be. When yDiff is negative, the 
	# slope is negative, angle is in 2nd quadrant. We want to convert 
	# angle to be the range from [0.0, 180) by converting the negative 
	# angles to be positive, 2nd-quadrant angles
	if yDiff < 0:
	    angle = angle + 180
	return angle
    
        
  
    def _chooseFinalMatches(self, matchQueues):
	"""Does greedy matching of lines from this to lines from the other, using the
	array of priority queues."""
	done = False
	usedThis = set()
	usedOther = set()
	finalMatches = [None] * len(self.lines)
	while not done:
	    i = 0
	    bestThis = -1
	    bestOther = -1
	    bestScore = 0.0
	    while i < len(self.lines):
		if matchQueues[i].isEmpty():
		    # if queue for current line i is empty, skip it and go to the next i
		    i += 1
		else:
		    (j, score) = matchQueues[i].firstElement()
		    if j in usedOther:
			# if the other line was already matched to a different one of
			# my line segments, then remove it from this queue and try again
			# NOTE: No increment of i here!
			matchQueues[i].delete()
		    elif (bestThis < 0) or (score < bestScore):
			# if this is the best score seen so far (or the first valid one)
			# then update trackers and go to the next i
			bestThis = i
			bestOther = j
			bestScore = score
		    else:
			# this isn't an option, leave it alone and go on
			i += 1
	    if bestThis == -1:
		# if while loop ends without finding anything, then there's nothing else to
		# do
		done = True
	    else:
		# We found a best score. Mark this segment and its match as used, add pair to
		# finalMatches, and pop queue until empty?
		usedThis.add(bestThis)
		usedOther.add(bestOther)
		finalMatches[bestThis] = (bestOther, bestScore)
		matchQueues[bestThis].clear()
	return finalMatches
    
    
    def partitionLines(self):
	"""Takes the current set of lines, and computes all the angles associated with them.
	It then uses the partition algorithm to divide them into similar clusters, based
	on the _similarLines method. Next it computes the "representative" line for the cluster."""
	angles = self._calcAllAngles(self.lines)
	angles = angles.reshape( (-1, 1) )
	lineData = np.hstack( (self.lines, angles) )
	labels = partitionAlg.partition(lineData, self._similarLines)

	repLines = np.empty( (0,5), dtype=np.int32)
	for i in range(labels.min(), labels.max() + 1):
	    clusterLines = lineData[labels.ravel() == i]
	    repLine = self._mergeCluster(clusterLines)
	    repLine = repLine.astype(np.int32)
	    repLines = np.vstack( (repLines, repLine) )

	self.lines = repLines
	

    def _similarLines(self, lineData1, lineData2):
	"""Takes two arrays giving end points of lines. Each has four values. Two\
	lines are similar if (a) their angle is close to the same and either (b) both endpoints
	are close or (c) one end overlaps with the end of the other."""
	# separate line endpoints from angles
	line1 = lineData1[0:4]
	line2 = lineData2[0:4]
	angle1 = lineData1[4]
	angle2 = lineData2[4]
	
	# check if angles are similar enough (must be 3 degrees or less)
	if abs(angle1 - angle2) > 4:  # not similar if angles differ by more than degrees
	    return False

	# check if perpendicular distance from line midpoints to other line are close enough
	pDist1 = self._perpDistance(line1, line2)
	pDist2 = self._perpDistance(line2, line1)
	if (pDist1 + pDist2 >= 8):
	    return False
	    
	# check if endpoints of lines are close enough to each other
	distA = self._euclideanDist(line1[0:2], line2[0:2])
	distB = self._euclideanDist(line1[0:2], line2[2:4])
	distC = self._euclideanDist(line1[2:4], line2[0:2])
	distD = self._euclideanDist(line1[2:4], line2[2:4])
	
	if (distA < 10) or (distB < 10) or (distC < 10) or (distD < 10):
	    return True

	# check if shorter line overlaps along longer lines axis
	line1Len = self._euclideanDist(line1[0:2], line1[2:4])
	line2Len = self._euclideanDist(line2[0:2], line2[2:4])
	if line1Len <= line2Len:
	    refLine = self._rotateAxes(line2, angle2)
	    compLine = self._rotateAxes(line1, angle2)
	else:
	    refLine = self._rotateAxes(line1, angle1)
	    compLine = self._rotateAxes(line2, angle1)
	
	refXs = refLine[ [0, 2] ]
	minRefX = refXs.min()
	maxRefX = refXs.max()
	compXs = compLine[ [0, 2] ]
	if np.any( np.logical_and(minRefX <= compXs, compXs <= maxRefX) ):
	    return True 
	else:
	    return False


    def _perpDistance(self, line1, line2):
	"""Given two lines, it determines the perpendicular distance from the middle of line1
	to line2."""
	(px, py) = np.mean( line1.reshape( (2, 2) ), axis = 0 )
	[x1, y1, x2, y2] = line2
	if x1 == x2:  # special case for vertical lines
	    return abs(px - x1)
	slope = float(y2 - y1) / (x2 - x1)
	A = slope
	B = -1
	C = y1 - (slope * x1)
	pDist = abs(A * px + B * py + C) / np.sqrt(A**2 + B**2)
	return pDist
    
    
    def _rotateAxes(self, line, angle, toNew = True):
	"""Takes in a line (which has two (x, y) endpoints and potentially
	an angle, but the angle is ignored. It also takes in the angle to rotate the axes by.
	It has an optional argument, toNew, that is true when we are converting a
	line from the original axes to the new rotated ones, and it is false when
	we are converting a line in the rotated axes back to the original ones.
	It computes and returns the new line."""
	theta = np.radians(angle)
	rotToNew = np.array([[np.cos(theta), -np.sin(theta)], 
	                     [np.sin(theta),  np.cos(theta)]])
	rotToOld = rotToNew.T
	if toNew:
	    rotMatr = rotToNew
	else:
	    rotMatr = rotToOld
	oldPt1 = line[0:2].reshape( (-1, 1) )
	newPt1 = np.matmul(rotMatr, oldPt1)
	oldPt2 = line[2:4].reshape( (-1, 1) )
	newPt2 = np.matmul(rotMatr, oldPt2)
	newLine = np.concatenate( (newPt1.ravel(), newPt2.ravel()) )
	return newLine
    
    def _mergeCluster(self, clust):
	"""Takes in a cluster, a matrix where rows contain lines given as
	two (x, y) endpoints plus the angle of the line, and it computes
	a single representative line to take the place of the cluster.
	Returns that line."""
	#print "Cluster:", clust.shape
	#print clust
	if len(clust) == 1:
	    #print "ONE LINE", clust.shape, clust[0]
	    return clust[0]
	avgAng = np.mean(clust[:,4])

	def doRot(line):
	    return self._rotateAxes(line, avgAng)

	linesNewAxis = np.apply_along_axis(doRot, 1, clust)

	xValues = linesNewAxis[:, [0, 2]]
	yValues = linesNewAxis[:, [1, 3]]
	minX = xValues.min()
	maxX = xValues.max()
	avgY = yValues.mean()
	newLine = np.array( [minX, avgY, maxX, avgY] )
	newOrig = self._rotateAxes(newLine, avgAng, toNew = False)
	newOrig = newOrig.astype(np.int32)
	#blank = np.zeros( (400, 400, 3), dtype=np.uint8)
	#for i in range(len(clust)):
	    #line = clust[i].astype(np.int32)
	    #cv2.line(blank, (line[0], line[1]), (line[2], line[3]), (0, 255, 255))
	    #cv2.imshow("FindAvg", blank)
	#time.sleep(0.2) #cv2.waitKey(0)
	#cv2.line(blank, (newOrig[0], newOrig[1]), (newOrig[2], newOrig[3]), (255, 255, 0))
	#cv2.imshow("FindAvg", blank)
	#time.sleep(0.2) #cv2.waitKey(0)
	return np.append(newOrig, avgAng)

    
    def testPerpDist(self):
	formText = "{0:d} {1:d} {2:4.1f} {3:4.1f}"
	for i in range(len(self.lines)):
	    line1 = self.lines[i]
	    angle1 = self._calcAngle(line1)
	    for j in range(i+1, len(self.lines)):
		line2 = self.lines[j]
		angle2 = self._calcAngle(line2)
		angleDiff = abs(angle1 - angle2)
		if angleDiff < 5:
		    pDist1 = self._perpDistance(line1, line2)
		    if pDist1 <= 4:
			nim = self.image.copy()
			cv2.line(nim, (line1[0], line1[1]), (line1[2], line1[3]), (255, 255, 0))
			cv2.line(nim, (line2[0], line2[1]), (line2[2], line2[3]), (0, 255, 255))
			text = formText.format(i, j, pDist1, angleDiff)
			cv2.putText(nim, text, (20, 220), cv2.FONT_HERSHEY_SIMPLEX, 0.6, (255, 0, 0))
			cv2.imshow("NIM", nim)
			cv2.waitKey(0)    


if __name__ == '__main__':
    name = "PioneerPicsSummer08/foo{:0>4d}.jpeg"
    logger = OutputLogger.OutputLogger(True, True)
    picNum = 45
    while True:
	print "--------------------------", picNum, "---------------------------"
	#nextStr = raw_input("Enter number of first picture: ")
	#if nextStr == 'q':
	    #break
        nextName = name.format(picNum) #int(nextStr))
        #print "Comparing", nextName
        image = cv2.imread(nextName)    
        cv2.imshow("Original", image)
	cv2.moveWindow("Original", 100, 0)
        h1 = HoughLines(image, logger)
        h1.displayFeaturePics("Original-Hough", 100, 300)
	cv2.waitKey(0)
	#h1.testPerpDist()
	#h1.partitionLines()
	picNum += 1
	
	#nextStr = raw_input("Enter number of second picture: ")
	#if nextStr == 'q':
	    #break
	#name2 = name.format(int(nextStr))
	##print "   to", name2
	#im2 = cv2.imread(name2)	
	#cv2.imshow("Other", im2)
	#cv2.moveWindow("Other", 400, 0)
	#h2 = HoughLines(im2, logger)	
	#h2.displayFeaturePics("Other-Hough", 400, 300)
	#res = h1.evaluateSimilarity(h2)
	#print "SIM =", res
	#cv2.waitKey(30)
    cv2.destroyAllWindows()