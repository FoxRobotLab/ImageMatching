""" ========================================================================
 * ImageSegmentation.py
 *
 *  Created on: May 2016
 *  Author: susan
 *
 * The ImageSegmentation object contains information about the. segmenting of
 * the image using Watershed. It matches segments to each other by finding the
 * best match of the four points.
 *
========================================================================="""


import math
import cv2
import numpy as np
import FeatureType
import FoxQueue
import OutputLogger


class ImageSegmentation(FeatureType.FeatureType):
    """Holds data about Hough Lines found in the input picture."""
    def __init__(self, image, logger):
        """Takes in an image and a logger and builds the hough lines. Also
        initializes other instance variables that are needed"""
	FeatureType.FeatureType.__init__(self, image, logger, 120.0, 200.0)
        self.foundWatersheds = 0
	self.wshedImage = self.image.copy()
	self._makeColorMapping()
	self._computeWatershed()



    def _computeWatershed(self):
	"""Set up helper images, run watershed algorithm, limit to the most\
	significant watersheds, and display the result"""
	#distMat = np.array( self.image.shape, np.float32 )
	workImg = cv2.GaussianBlur(self.image, (5, 5), sigmaX = 0, sigmaY = 0)
	
	element = cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (3, 3))
	simplified = cv2.morphologyEx(workImg, cv2.MORPH_OPEN,
	                              element, iterations = 2)
	simplified = cv2.morphologyEx(simplified, cv2.MORPH_CLOSE,
	                              element, iterations = 2)
	#simplified = workImg 
	grayImg = cv2.cvtColor(simplified, cv2.COLOR_BGR2GRAY)
	img_gray = cv2.cvtColor(grayImg, cv2.COLOR_GRAY2BGR)
	
	cannyImage = cv2.Canny(grayImg, 10, 70, 3)
	cannyImage = cv2.morphologyEx(cannyImage, cv2.MORPH_CLOSE, element)
	cannyImage = cv2.dilate(cannyImage, element)
	sureBG = cannyImage
	nonCanny = cv2.bitwise_not(cannyImage)
	distMat = cv2.distanceTransform(nonCanny, cv2.DIST_L1, 3)
	dMat = distMat.astype(np.uint8)
	ret, sureSegments = cv2.threshold(dMat, dMat.mean(), 255, 0)
	unknown = cv2.subtract(sureBG, sureSegments)
	ret, markers = cv2.connectedComponents(sureSegments)
	markers = markers + 1  # increase marked regions so background is 1, not zero
	markers[unknown == 255] = 0
	if markers.max() > 255:
	    print "WARNING: TOO MANY REGIONS!!@"
	markerImg = (markers * 255) / markers.max()
	markerImg = markerImg.astype(np.uint8)
	markerImg1 = cv2.applyColorMap(markerImg, cv2.COLORMAP_HOT)


	cv2.imshow("SureBack", sureBG)
	cv2.moveWindow("SureBack", 0, 0)
	cv2.imshow("SureFront", sureSegments)
	cv2.moveWindow("SureFront", 400, 0)
	cv2.imshow("Unknown", unknown)
	cv2.moveWindow("Unknown", 0, 300)
	cv2.imshow("Working RES", workImg)
	cv2.moveWindow("Working RES", 400, 300)
	cv2.imshow("Markers Before", markerImg1)
	cv2.moveWindow("Markers Before", 0, 600)
	cv2.waitKey(0)
	
	markersAfter = cv2.watershed(workImg,markers)
	workImg[markersAfter == -1] = [255,255,255]	
	if markersAfter.max() > 255:
	    print "WARNING: TOO MANY REGIONS!!@"
	markerImg = (markersAfter * 255) / markersAfter.max()
	markerImg = markerImg.astype(np.uint8)
	markerImg2 = cv2.applyColorMap(markerImg, cv2.COLORMAP_JET)
	
	    
	cv2.imshow("Markers After", markerImg2)
	cv2.moveWindow("Markers After", 400, 600)
	
	#cv2.imshow("Simplified", simplified)
	#cv2.moveWindow("Simplified", 100, 0)
	#cv2.imshow("gray version", grayImg)
	#cv2.moveWindow("gray version", 400, 0)
	#cv2.imshow("Canny", cannyImage)
	#cv2.moveWindow("Canny", 100, 300)
	#cv2.imshow("NonCanny", nonCanny)
	#cv2.moveWindow("NonCanny", 400, 300)
	#cv2.imshow("Distance Transform", dMat)
	#cv2.moveWindow("Distance Transform", 100, 600)
	#cv2.imshow("Sure Sure", sureSegments)
	#cv2.moveWindow("Sure Sure", 400, 600)
	#cv2.waitKey(20)
	#self._findWatersheds()  # Inputs?
	#self._collectSignifWatersheds() 
	
	#self.wshedImage = cv2.addWeighted(self.wshedImage, 0.5,
	                                  #img_gray, 0.5)
	
	
	
	
	
    def _makeColorMapping(self):
	"""It generates a random color for each slot in a table
	matrix. Not quite sure why I need this. 500 random colors."""
	pass

			
	 
 
    def displayFeaturePics(self, windowName, startX, startY):
        """Given a window name and a starting location on the screen, this creates
        an image that represents the color signature and displays it."""
	pass		    
    


    def evaluateSimilarity(self, otherHL):
        """Given another HoughLines object, evaluate the similarity. 
        Starts by making a priority queue for each line here.
        It then adds each line in the otherLines to queues for the lines that match.
        It then uses a greedy algorithm to pick matches (not optimal, but efficient).
        """
	numLines = len(self.lines)
	otherLines = otherHL.lines
	numOtherLines = len(otherLines)
        matchQueues = [None] * numLines
        for thisPos in range(numLines):
            matchQueues[thisPos] = FoxQueue.PriorityQueue()
            for thatPos in range(numOtherLines):
                nextScore = self._compareLines(self.lines[thisPos], otherLines[thatPos])
                if nextScore >= 0.0:
                    matchQueues[thisPos].insert(thatPos, nextScore)
            
        #for i in range(numLines):
	    #self.logger.log("Matches to line " + str(i))
	    #self.logger.log("  " + str(matchQueues[i].getSize()) + "matches")
	    #self.logger.log("  Best match: " + str(matchQueues[i].firstElement()))
	
	finalMatches = self._chooseFinalMatches(matchQueues)
	
	#self.logger.log("Final matches:")
	#for i in range(numLines):
	    #freshIm1 = self.image.copy()
	    #freshIm2 = otherHL.image.copy()
	    #val = finalMatches[i]
	    #if val == None:
		#self.logger.log("  " + str(i) + " None")

	    #else: 
		#(j, s) = val
		#self.logger.log("  " + str(i) + " " + str(j) + " " + str(s))
		#[x1, y1, x2, y2] = self.lines[i]
		#cv2.line(freshIm1, (x1, y1), (x2, y2), (255, 255, 0), 2)
		#print self.lines[i], (x1, y1), (x2, y2)
		#[x1, y1, x2, y2] = otherLines[j]
		#cv2.line(freshIm2, (x1, y1), (x2, y2), (255, 255, 0), 2)
		#cv2.imshow("Matches 1", freshIm1)
		#cv2.imshow("Matches 2", freshIm2)
		#cv2.waitKey(0)
		
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
	#print "A =", numLines, "  B =", numOtherLines
	#print "Total Lines =", numLines + numOtherLines
	#print "matchCount =", matchCount
	#print "unmatched =", unmatched
	#print "sum       =", matchCount + unmatched
	#print "penalty =", penalty
	#print "totalScore =", totalScore
	#print "aveScore =", aveScore 
	#self.logger.log("Evaluate similarity, result is:" + str(aveScore))
	
	return self._normalizeSimValue(aveScore)
    
    


    def _compareLines(self, lineA, lineB):
        """A helper for evaluateSimilarity, it takes two lines and computes
        their similarity"""
        [xa1, ya1, xa2, ya2] = lineA
        [xb1, yb1, xb2, yb2] = lineB
        leftLeftX = abs(xa1 - xb1)
        leftLeftY = abs(ya1 - yb1)
        rightRightX = abs(xa2 - xb2)
        rightRightY = abs(ya2 - yb2)
        totalOne = math.sqrt(leftLeftX ** 2 + leftLeftY ** 2 + 
                             rightRightX ** 2 + rightRightY ** 2)
        
        leftRightX = abs(xa1 - xb2)
        leftRightY = abs(ya1 - yb2)
        rightLeftX = abs(xa2 - xb1)
        rightLeftY = abs(ya2 - yb1)
        totalTwo = math.sqrt(leftRightX ** 2 + leftRightY ** 2 +
                             rightLeftX ** 2 + rightLeftY ** 2)
        endPtDiff = min(totalOne, totalTwo)
        
        if endPtDiff > 100:  # if too different already, no match!
            return -1.0
        
        angleA = self._calcAngle(lineA)
        angleB = self._calcAngle(lineB)
        angleDiff = abs(angleA - angleB)
        if angleDiff > 30: # if too different then no match!
            return -1.0
	
        return endPtDiff + angleDiff
       
       
       
    def _calcAngle(self, line):
        """Given a line, it computes the angle of the line in degrees, in the
        range [0, 180)"""
        xDiff = line[2] - line[0]           # points are sorted by x value
        yDiff = line[1] - line[3]           # top of screen is zero
        angle = math.atan2(yDiff, xDiff) * (180 / numpy.pi)
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
    



if __name__ == '__main__':
    name = "PioneerPicsSummer08/foo{:0>4d}.jpeg"
    logger = OutputLogger.OutputLogger(True, True)
    while True:
	nextStr = raw_input("Enter number of first picture: ")
	if nextStr == 'q':
	    break
        nextName = name.format(int(nextStr))
        #print "Comparing", nextName
        image = cv2.imread(nextName)    
        cv2.imshow("Original", image)
	cv2.moveWindow("Original", 0, 0)
        h1 = ImageSegmentation(image, logger)
        #h1.displayFeaturePics("Original-Hough", 100, 300)
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
	#h1.evaluateSimilarity(h2)
	cv2.waitKey(30)
    cv2.destroyAllWindows()