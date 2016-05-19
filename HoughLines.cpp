/*
 * HoughLines.cpp
 *
 *  Created on: Jun 8, 2011
 *      Author: susan
 *
 *  The HoughLines object contains information about lines found in the image using the cvCanny edge detection and
 *  Hough lines detection algorithms.  It copies the lines from the CvSeq format they are found in to a vector
 *  of vectors of CvPoints, for easier use.  It compares similarity between HoughLines objects by matching lines
 *  to each other, trying to maximize the similarity of matched lines.
 *
 */

#include "HoughLines.h"



/*
 * Constructor takes an IplImage * and generates the Hough Lines for it, using cvCanny and the probabilistic
 * Hough Lines cvHoughLines
 */
HoughLines::HoughLines(IplImage* newImage, OutputLogger* op) {
	image = newImage;
	outputter = op;
	findHoughLines();
}



/*
 * Copy constructor that takes in one object and copies it to the new one
 */
HoughLines::HoughLines(HoughLines* other) {
	image = other->image;
	numLines = other->numLines;
	for ( int i = 0; i < numLines; i++ ) {
		lines[i][0] = other->lines[i][0];
		lines[i][1] = other->lines[i][1];
	}
}


/*
 * Default destructor... because the image comes in from elsewhere and it isn't
 * copied, there isn't anything to do here?
 */
HoughLines::~HoughLines() {
	for ( int i = 0; i < numLines; i++ ) {

	}
}




/*
 * Finds canny edges and hough lines for the given image, initializing the lines instance variable
 * to have the resulting endpoints of the lines.
 */
void HoughLines::findHoughLines() {
	if ( image == NULL ) {
		*outputter << "HoughLines:   no image" << endl;
		outputter->writeLog();
		return;
	}
	CvMemStorage* storage = cvCreateMemStorage(0);
	CvSeq* houghLines = NULL;
	IplImage* cannyImage = cvCreateImage( cvGetSize(image), 8, 1 );
	cvCvtColor(image, cannyImage, CV_RGB2GRAY);
	cvCanny(cannyImage, cannyImage, 10, 70, 3);

	houghLines = cvHoughLines2( cannyImage,
			storage,
			CV_HOUGH_PROBABILISTIC,
			1,
			CV_PI/180,
			100,
			30,
			2 );

	//add cv points to lines
	for(int i = 0; i < houghLines->total; i++ )
	{
		CvPoint* line = (CvPoint*)cvGetSeqElem(houghLines,i);
		lines.push_back(vector<CvPoint>(2));
		lines[i][0] = line[0];
		lines[i][1] = line[1];
	}

    //sort the points into x-most-left order
	numLines = houghLines -> total;
	sortCvPoints();

	// Clean up things that were created here and are no longer needed
	cvReleaseMemStorage( &storage );
	cvReleaseImage(&cannyImage);
}




/*
 * Takes in another HoughLines object and evaluates the similarity between the Hough Lines
 */
double HoughLines::evaluateSimilarity(HoughLines* otherLines) {
	int thisSize = lines.size();
	int otherSize = otherLines->lines.size();
	MatchPQ matchQueues[thisSize];

	// loop through every line in this set.  For each line, consider every line in the other set.
	// If that other line matches this one add it to this one's priority queue,
	// ordered in increasing order by similarity score
	for( int cnt1 = 0; cnt1 < thisSize; cnt1++ ) {
		for ( int cnt2 = 0; cnt2 < otherSize; cnt2++ ) {
			double nextScore = compareLines(lines[cnt1], otherLines->lines[cnt2]);
			if ( (nextScore >= 0.0) ) {
				CompareHough* matchInfo = new CompareHough(cnt2, nextScore);
				matchQueues[cnt1].push(matchInfo);
			}
		}  // for cnt2
 	} // for cnt1

	// create an array to hold the actual matches CompareWatershed objects contain the score, the indices, and the watershedInfo object
	// itself).  Initialize the array to have NULL values.
	CompareHough** finalMatches = new CompareHough*[thisSize];
	for ( int i = 0; i < thisSize; i++) {
		finalMatches[i] = NULL;
	}
	// greedy algorithm to pick final matches.  It picks the match with the overall best score, then removes that
	// contour from consideration and repeats until no more matches appear.
	chooseFinalMatches(matchQueues, finalMatches);


	int matchCount = 0;
	double totalScore = 0.0;
	for (int i = 0; i < thisSize; i++) {
		if (finalMatches[i] != NULL) {
			matchCount += 2;
//			int otherPos = finalMatches[i]->getPos();
			double score = finalMatches[i]->getScore();
//			*outputter << "Line " << i << " matches other line " << otherPos << " with score " << score;
//			outputter->writeLog();
			totalScore += score;
		}
	}

	/* Since not every line matches another, we're going to double-count the score, one for each line participating in
	 * the match, and then we'll add a penalty value for each line that is unmatched
	 */
//	*outputter << "Matched " << matchCount << " out of " << thisSize + otherSize << endl;
	int unmatched = (thisSize + otherSize) - matchCount;
	double penalty = 130 * unmatched;
//	*outputter << "Total score = "  << 2 * totalScore << " plus penalty " << penalty << endl;
	double aveScore = (2 * totalScore + penalty) / (thisSize + otherSize);
//	*outputter << "Average score = " << aveScore << endl;
//	outputter->writeLog();

	return aveScore;
}


/*
 * Helper for evaluateSimilarity
 * Takes in two lines, given as vectors containing two points.  The endpoints of the two
 * lines must be sorted so that the endpoint with the smaller x value should come first,
 * and if the x values are the same, then it would be nice if the endpoint with the smaller
 * y value came first.  This evaluates the similarity of the lines, looking at distance between
 * endpoints and the angles of the lines.  Even with lines sorted by x values, we still need to
 * consider matching each endpoint with each endpoint.  If we call the endpoints first and second, then
 * it is possible that the first endpoint of line 1 is closest to the second endpoint of line 2 and
 * the second of line 1 is closest to the first of line 2.  Therefore, this computes the x and y differences
 * between the endpoints, matching left to left and right to right AND matching left to right and right to
 * left.  It sums up the differences and keeps the smaller of the two differences.
 * It then computes the angles of each line, and determines the difference in angle, a value between 0 and 180
 * If the difference is greater than 30 degrees, then it adds in a penalty value.
 */
double HoughLines::compareLines(const vector<CvPoint> line1, const vector<CvPoint> line2) {

	double distance = 0.0;

	int leftLeftX = abs(line1[0].x - line2[0].x);
	int leftLeftY = abs(line1[0].y - line2[0].y);
	int rightRightX = abs(line1[1].x - line2[1].x);
	int rightRightY = abs(line1[1].y - line2[1].y);
	int totalA = sqrt((leftLeftX * leftLeftX) + (leftLeftY * leftLeftY)) + sqrt((rightRightX * rightRightX) + (rightRightY * rightRightY));

	int leftRightX = abs(line1[0].x - line2[1].x);
	int leftRightY = abs(line1[0].y - line2[1].y);
	int rightLeftX = abs(line1[1].x - line2[0].x);
	int rightLeftY = abs(line1[1].y - line2[0].y);
	int totalB = sqrt((leftRightX * leftRightX) + (leftRightY * leftRightY)) + sqrt((rightLeftX * rightLeftX) + (rightLeftY * rightLeftY));

	distance += min(totalA, totalB);

	if (distance > 100) { // then no match
		return -1.0;
	}
	double angle1 = calcAngle(line1);
	double angle2 = calcAngle(line2);
	double angleDiff = abs(angle1 - angle2);

	distance += angleDiff;
	if (angleDiff > 30) {  // then no match
		return -1;
	}

	return distance;
}


/*
 * Helper for compareLines
 * Given a line, it computes the angle of the line, as a value in the range [0, 180)
 * and returns the value in degrees.
 */
double HoughLines::calcAngle(const vector<CvPoint> line) {
	double xDiff = line[1].x - line[0].x;   // because points are sorted by y value
	double yDiff = line[0].y -  line[1].y;  // because top of screen is zero
	double angle = atan2(yDiff, xDiff) * (180 / PI);
	// angle is in the range [-90.0,90).  xDiff can never be negative by sorting of points
	// but yDiff may be.  When yDiff is negative, the slope is negative, angle is in 2nd quadrant
	// we want to convert angle to be the range from [0.0, 180) by converting the negative angles
	// to be positive, 2nd-quadrant angles
	if (yDiff < 0) {
		angle = angle + 180;
	}
	return angle;
}



/*
 * Helper for evaluateSimilarity, given an array of priority queues, one queue for each line in the
 * HoughLines object, it picks the best overall match to make, then removes that line from further
 * consideration and repeats until no more matches can be made.  It is a greedy process.
 */
void HoughLines::chooseFinalMatches(MatchPQ* matchQueues, CompareHough** finalMatches) {
	int totalSize = lines.size();
	vector<int> usedThis;
	vector<int> usedOther;
	bool done = false;

	while ( !done ) {
		int i = 0;
		int bestThis = -1;
		int bestOther = -1;
		double bestScore = 0.0;

		while ( i < totalSize ) {
			if ( matchQueues[i].empty() )  {
				// if queue for current i is empty, skip it and go to the next i
				i++;
			}
			else {
				CompareHough* nextInfo = (CompareHough*)matchQueues[i].top();
				int nextPos = nextInfo->getPos();
				double nextScore = nextInfo->getScore();
				if ( inUse(nextPos, usedOther) ) {
					// if this other segment is already assigned to some other of my segments, then
					// remove it from this queue and continue... NOTE NO i++ here!
					matchQueues[i].pop();
				}
				else if ( (bestThis < 0) || (nextScore < bestScore) ) {
					// if this is the best score seen so far (or the first valid one), then update trackers
					// and go to the next i
					bestThis = i;
					bestOther = nextPos;
					bestScore = nextScore;
					i++;
				}
				else {
					// this info isn't what we want, we want to go to the next i and continue
					i++;
				}
			}
		} // while i < totalSize
		if ( bestThis == -1 ) {
			done = true;
		}
		else {
			// We found a best score.  Mark this segment and its match as used, move the CompareWatershed
			// object to the finalMatches site, and pop the current queue until it is empty
			usedThis.push_back(bestThis);
			usedOther.push_back(bestOther);
			finalMatches[bestThis] = (CompareHough*)matchQueues[bestThis].top();
			while ( !matchQueues[bestThis].empty() ) {
				matchQueues[bestThis].pop();
			} // while
		} // else
	} // while !done
}

/*
 * Helper for makeFinalMatches, it determines if a given line is already assigned to some other match
 */
bool HoughLines::inUse(int pos, vector<int>inUse) {
	for ( unsigned int i = 0; i < inUse.size(); i++ ) {
		if ( inUse[i] == pos ) {
			return true;
		}
	}
	return false;
}




/**
 * sortCvPoints: sorts the points in lines so that they're ordered by leftmost points
 * Uses Insertion sort because of the rather small data sets it handles
 */
void HoughLines:: sortCvPoints(){
	for(int i =0; i < numLines; i++){
		for(int q = i; q < numLines; q++){
			if(firstPointBefore(lines[q], lines[i])){
				//swaps the points
				lines[q].swap(lines[i]);
			}
		}
	}
}



/**
 * compareCvPoint: compares CvPoints in this case it returns comparisons based on
 * which CvPoint is leftmost
 */
bool HoughLines:: firstPointBefore(vector<CvPoint> p1, vector<CvPoint> p2){
	if ( p1[0].x < p2[0].x ) {
		return true;
	}
	else if ( (p1[0].x == p2[0].x) && (min(p1[0].y, p1[1].y) < min(p2[0].y, p2[1].y))) {
		return true;
	}
	else {
		return false;
	}
}



/*
 *  displays the hough lines on a copy of the image, if there is one
 */
void HoughLines::displayFeaturePics(const char* windowName, int startX, int startY) {
	IplImage* displayPic = cvCloneImage(image);
	for (unsigned int i = 0; i < lines.size(); i++) {
		CvPoint pt1 = lines[i][0];
		CvPoint pt2 = lines[i][1];
		cvLine(displayPic, pt1, pt2, CV_RGB(255, 0, 0), 2, 8);
	}
	cvNamedWindow(windowName);
	cvMoveWindow(windowName, startX, startY);
	cvShowImage(windowName, displayPic);
}
