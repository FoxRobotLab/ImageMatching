/*
 * WatershedInfo.cpp
 *
 *  Created on: Jun 23, 2011
 *      Author: susan
 *
 *  The WatershedInfo class contains information about a single segment found by the Watershed or Waterfall algorithms
 *  It keeps track of the bounding rectangle for the segment, an image containing the segment itself, the contour
 *  of points that form the segment's outline, and the spatial and central moments of the segment.  It also has a
 *  similarity evaluation method for comparing one segment to another based on the bounding rectangle, overlapping points,
 *  and moments.
 */

#include "WatershedInfo.h"


/*
 * Given the image containing just this segment, and the output logger, this constructor
 * sets up all the information including threshold values.  It counts the pixels, determines
 * if the segment is big enough, constructs the contour, bounding rectangle, and moments
 */
WatershedInfo::WatershedInfo(IplImage* wshedImage, OutputLogger* op) {
	// Initialize everything
	outputter = op;
	storage = cvCreateMemStorage(0);
	watershedImage = cvCloneImage(wshedImage);

	pixelThreshold = 0.00651;
	bboxThreshold = 0.03256;
	int imageArea = watershedImage->height * watershedImage->width;
	contour = NULL;

	// Compute size in pixels, and decide if it is a big enough perc of the image size
	// If not, don't bother with the expensive computations of other features
	sizeInPixels = cvCountNonZero(watershedImage);
	double pixelPerc = ((double) sizeInPixels) / imageArea;
	if ( pixelPerc < pixelThreshold ) {
		bigEnough = false;

	}
	else {
		// Find the contours and bounding rectangle
		IplImage* contourCopy = cvCloneImage(watershedImage);
		cvFindContours(contourCopy, storage, &contour, sizeof(CvContour),
			CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE );

		boundingRect = cvBoundingRect(contour);

		// Compute the size of the bounding rectangle, and decide if it is a big enough
		// percent of the image size.
		// If not, don't bother computing moments
		int boundingArea = boundingRect.width * boundingRect.height;
		double boundingPerc = ((double) boundingArea) / imageArea;
		if ( boundingPerc < bboxThreshold ) {
			bigEnough = false;
		}
		else {
			cvMoments(watershedImage, &moments);
			bigEnough = true;
		}

		cvReleaseImage(&contourCopy);
	}
}



/*
 * This must release the storage and the watershedImage that was copied.  The contour was allocated
 * in the storage, so releasing storage takes care of that.
 */
WatershedInfo::~WatershedInfo() {

	cvReleaseMemStorage(&storage);
	cvReleaseImage(&watershedImage);

}


/*
 * Returns the value of bigEnough, which was set in the constructor
 */
bool WatershedInfo::isSignificant() {
	return bigEnough;
}


/*
 * Returns the contour that was found
 */
CvSeq* WatershedInfo::getContour() {
	return contour;
}


/*
 * Access and return the moment m_(x, y)
 */
double WatershedInfo::getMomentAt(int x, int y) {
	if ((0 <= x) && (x < 4) && (0 <= y) && (y < 4 - x)) {
		return cvGetSpatialMoment(&moments, x, y);
	}
	return -1.0;
}


/*
 * Return the size of the segment in pixels
 */
int WatershedInfo::getSizeInPixels() {
	return sizeInPixels;
}




/*
 * Evaluating similarity requires accessing the spatial and central moments, whose numbers
 * are as follows:
 * If x = ???, then y can take on the values ???
 * If x = 0, then y = 0, 1, 2, 3
 * If x = 1, then y = 0, 1, 2
 * If x = 2, then y = 0, 1
 * If x = 3, then y = 0
 *
 * This method takes in another WatershedInfo object, and it compares them.  It performs
 * a series of checks to see if the segments are similar enough:
 * 1. Do the bounding rectangles overlap enough?
 * 2. Do the segments actually overlap in at least some pixels?
 * 3. Consider the overlapping pixels as a percentage of each segment... are they close enough to the same
 *    percentage?  (within 0.4)?  (This eliminates a tiny segment matching a big one that encompasses it)
 * If any of these tests fail, then the method returns -1.0, meaning no match
 * Otherwise it calculates the difference between each of the spatial and central moments from each segment.
 * It scales the difference by the size of the two moments, and adds up the differences.
 */
double WatershedInfo::evaluateSimilarity(WatershedInfo* otherInfo) {
	double spatialMomDiffSum = 0.0;
	double centralMomDiffSum = 0.0;
	double myPerc;

	// If the bounding rectangles don't overlap enough, then stop and return -1.0
	if ( !rectanglesOverlap(boundingRect, otherInfo->boundingRect) ) {
		return -1.0;
	}
	else {
		IplImage* overlap = cvCloneImage(watershedImage);
		cvAnd(watershedImage, otherInfo->watershedImage, overlap);

		int countOverlap = cvCountNonZero(overlap);
		// If no actual overlap, then go no further
		if ( countOverlap == 0 ) {
			return -1.0;
		}
		int myCount = cvCountNonZero(watershedImage);
		int otherCount = cvCountNonZero(otherInfo->watershedImage);
		myPerc = ((double)countOverlap) / myCount;
		double otherPerc = ((double)countOverlap) / otherCount;
		// If overlap percentages are too different, stop and return -1.0

		if ( abs(myPerc - otherPerc) > 0.4) {
			return -1.0;
		}
		else {
			// Segments are similar enough, go ahead
			// Compute the sum of the differences between each moment and the corresponding one, scaled by
			// the size of each moment.  Do this for both spatial and central, so long as the moments themselves
			// are not zero

			for (int x = 0; x < 4; x++) {
				for (int y = 0; y < (4 - x); y++) {
					double mom1 = cvGetSpatialMoment(&moments, x, y);
					double mom2 = cvGetSpatialMoment(&(otherInfo->moments), x, y);
					if (mom1 != 0.0 && mom2 != 0.0) {
						double spatialMomDiffByMe = abs(mom1-mom2) / mom1;
						double spatialMomDiffByOther = abs(mom1-mom2) / mom2;
						spatialMomDiffSum += spatialMomDiffByMe + spatialMomDiffByOther ;
					}
					mom1 = cvGetCentralMoment(&moments, x, y);
					mom2 = cvGetCentralMoment(&(otherInfo->moments), x, y);
					if (mom1 != 0.0 && mom2 != 0.0) {
						double centralMomDiffByMe = abs(mom1-mom2) / mom1;
						double centralMomDiffByOther = abs(mom1-mom2) / mom2;
						centralMomDiffSum += centralMomDiffByMe + centralMomDiffByOther ;
					}
				}
			}
			//		*outputter << "Moments similarity = " << spatialMomDiffSum << " " << centralMomDiffSum ;
			//		outputter->writeLog();

			// Final similarity score has four parts
			// First: percentage of non-overlap compared to this segment, scaled to 0.0 to 100.0
			double myOverPercScore = (1.0 - myPerc) * 100;
			// Second: percentage of non-overlap compared to other segment, scaled to 0.0 to 100.0
			double otherOverPercScore = (1.0 - otherPerc) * 100;
			// Third: the spatial moment score, just the absolute value of the sum of the differences
			double spatialMomentScore = abs(spatialMomDiffSum);
			// Fourth: the central moment score, which must be first bounded between -400.0 and 400.0, and then scaled
			//         so that positive values are scaled in half and negative ones are just made positive
			double centralMomentScore;
			if (centralMomDiffSum < -400.0) {
				centralMomDiffSum = -400.0;
			}
			else if (centralMomDiffSum > 400.0) {
				centralMomDiffSum = 400.0;
			}

			if (centralMomDiffSum >= 0) {
				centralMomentScore = centralMomDiffSum / 2.0;
			}
			else {
				centralMomentScore = abs(centralMomDiffSum);
			}
			//		*outputter << "Moments scores = " << spatialMomentScore << " " << centralMomentScore ;
			//		outputter->writeLog();
			return myOverPercScore + otherOverPercScore + spatialMomentScore + centralMomentScore;
		}
	}
}





/* ===================================================================
 * Private methods below here
 * ===================================================================
 */




/*
 * This is a helper for evaluateSimilarity.
 * It takes two rectangles that represent the bounding box of this info and
 * the other, and determines to what extent they overlap.  If their overlap, as
 * a percentage of the size of each rectangle, is between 0.5 and 1.5, then they
 * overlap enough, otherwise they do not.
 */
bool WatershedInfo::rectanglesOverlap(CvRect rect1, CvRect rect2) {
	int xOver, yOver;
	int overlapArea, area1, area2;

	// compute overlaps in each direction
	xOver = overlapInDir(rect1.x, rect2.x, rect1.width, rect2.width);
	yOver = overlapInDir(rect1.y, rect2.y, rect1.height, rect2.height);


	overlapArea = xOver * yOver;
	area1 = rect1.width * rect1.height;
	area2 = rect2.width * rect2.height;
	double perc1 = (double)overlapArea / area1;
	double perc2 = (double)overlapArea / area2;
	return ( (0.5 <= perc1) && (perc1 <= 1.5) && (0.5 <= perc2) && (perc2 <= 1.5) );
}


/* A helper for rectanglesOverlap, it computes the amount of overlap in a given direction, given two values and
 * the widths of each rectangles
 */
int WatershedInfo::overlapInDir(int val1, int val2, int range1, int range2) {
	int valA, valB, valC, valD;
	int over;

	if (val1 < val2) {
		valA = val1;
		valB = val1 + range1;
		valC = val2;
		valD = val2 + range2;
	}
	else {
		valA = val2;
		valB = val2 + range2;
		valC = val1;
		valD = val1 + range1;
	}
	if ( valB < valC ) {
		// no overlap
		over = 0;
	}
	else if ( valB > valD ) {
		// complete overlap
		over = valD - valC;
	}
	else {
		over = valB - valC;
	}
	return over;
}




//
//void WatershedInfo::foobarPrintPixels() {
//	for ( int r = 0; r < imageHeight; r++ ) {
//		for ( int c = 0; c < imageWidth; c++ ) {
//			unsigned char idx = CV_IMAGE_ELEM(watershedImage, unsigned char, r, c);
//			cout << setfill(' ') << setw(4) << (unsigned short)idx;
//		}
//		cout << endl;
//	}
//}
