/*
 * ImageSegmentation.cpp
 *
 *  Created on: Jun 8, 2011
 *      Author: susan
 */

#include "ImageSegmentation.h"




ImageSegmentation::ImageSegmentation(IplImage* img, OutputLogger* op) {
	outputter = op;
	image = img;
	imageHeight = image->height;
	imageWidth = image->width;
	imageArea = imageHeight * imageWidth;
	foundWatersheds = 0;


	// Instance variables
	wshedImage = cvCloneImage( image );
	cannyImage = cvCreateImage( cvGetSize(image), 8, 1 );
	visMarkers = cvCreateImage( cvGetSize(image), 8, 1);
	startMarkers = cvCreateImage(cvGetSize( image ), 8, 1);

	// Initialize color table and storage
	color_tab = cvCreateMat( 1, 500, CV_8UC3 );
	makeColorMapping(color_tab);
	storage = cvCreateMemStorage(0);
	element = cvCreateStructuringElementEx(2, 2, 0, 0, CV_SHAPE_ELLIPSE);

	computeWatershed();
}


ImageSegmentation::~ImageSegmentation() {
	cvReleaseImage(&cannyImage);
	cvReleaseImage(&wshedImage);
	cvReleaseImage(&visMarkers);
	cvReleaseImage(&startMarkers);


	cvReleaseMat(&color_tab);
	cvReleaseMemStorage( &storage );
	cvReleaseStructuringElement(&element);

	// clear up the vector of watershed objects
	for (unsigned int i = 0; i < watersheds.size(); i++) {
		delete watersheds[i];
	}
	watersheds.clear();
}




double ImageSegmentation::evaluateSimilarity(ImageSegmentation* otherSeg, char option) {
	cout <<"test4"<<endl;
	int thisSize = watersheds.size();
	int otherSize = otherSeg->watersheds.size();
	MatchPQ matchQueues[thisSize];
	IplImage* displayPic;
	WatershedInfo* ourNextInfo;
	WatershedInfo* otherNextInfo;
//	int thisCovered = 0;
//	int otherCovered = 0;
//	int thisNotCovered = 0;
//	int otherNotCovered = 0;

	if (option == 'v') {
		displayPic = cvCreateImage(cvGetSize(wshedImage), 8, 3);
	}
	// loop through every contour in this segmentation.  For each contour,
	// consider every contour in otherSeg.  If that other contour matches this one
	// add it to this contour's priority queue, ordered in increasing order by similarity score
	cout <<"test5"<<endl;
	for( int cnt1 = 0; cnt1 < thisSize; cnt1++ ) {
		ourNextInfo = watersheds[cnt1];
		for ( int cnt2 = 0; cnt2 < otherSize; cnt2++ ) {
			otherNextInfo = otherSeg->watersheds[cnt2];
			double nextScore = ourNextInfo->evaluateSimilarity(otherNextInfo);
			if ( (nextScore >= 0.0) ) {
				cout <<"test7"<<endl;
				CompareWatershed* matchInfo = new CompareWatershed(otherNextInfo, cnt2, nextScore);
				matchQueues[cnt1].push(matchInfo);
//				*outputter << "Possible match " << cnt1 << " with " << cnt2 << " with score " << nextScore;
//				outputter->writeLog();
			}
		}  // for cnt2
 	} // for cnt1
	cout <<"test6"<<endl;
	// create an array to hold the actual matches CompareWatershed objects contain the score, the indices, and the watershedInfo object
	// itself).  Initialize the array to have NULL values.
	CompareWatershed** finalMatches = new CompareWatershed*[thisSize];
	for ( int i = 0; i < thisSize; i++) {
		finalMatches[i] = NULL;
	}
	// greedy algorithm to pick final matches.  It picks the match with the overall best score, then removes that
	// contour from consideration and repeats until no more matches appear.
	chooseFinalMatches(matchQueues, finalMatches);

	return computeTotalScore(finalMatches,otherSeg, option, displayPic);
}



IplImage* ImageSegmentation::getWatershedPic() {
	return wshedImage;
}

IplImage* ImageSegmentation::getCannyPic() {
	return cannyImage;
}

void ImageSegmentation::displayFeaturePics(const char* windowName, int startX, int startY) {
	cvNamedWindow(windowName);
	cvMoveWindow(windowName, startX, startY);
	cvShowImage(windowName, wshedImage);
}


/* =============================================================================================
 * Private methods after this point
 * =============================================================================================
 */

void ImageSegmentation::chooseFinalMatches(MatchPQ* matchQueues, CompareWatershed** finalMatches) {
	int totalSize = watersheds.size();
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
				CompareWatershed* nextInfo = (CompareWatershed*)matchQueues[i].top();
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
			finalMatches[bestThis] = (CompareWatershed*)matchQueues[bestThis].top();
			while ( !matchQueues[bestThis].empty() ) {
				matchQueues[bestThis].pop();
			} // while
		} // else
	} // while !done
}


bool ImageSegmentation::inUse(int pos, vector<int>inUse) {
	for ( unsigned int i = 0; i < inUse.size(); i++ ) {
		if ( inUse[i] == pos ) {
			return true;
		}
	}
	return false;
}



double ImageSegmentation::computeTotalScore(CompareWatershed** finalMatches, ImageSegmentation* otherSeg, char option, IplImage* displayPic) {
	int thisSize = watersheds.size();
	int otherSize = otherSeg->watersheds.size();
	int thisCovered = 0;
	int otherCovered = 0;
	int thisNotCovered = 0;
	int otherNotCovered = 0;
	double totalScore = 0.0;
	int countMatches = 0;
	bool othersUsed[otherSize];
	for ( int i = 0; i < otherSize; i++ ) {
		othersUsed[i] = false;
	}
	// This loop collects the overall score for the segments that match, and marks in the othersUsed array
	// which segments in the other segmentation were used or not. It also collects the number of pixels for
	/// segments that match and for this segments that don't match.
	for ( int i = 0; i < thisSize; i++) {
		if ( finalMatches[i] != NULL ) {
			CompareWatershed* match = finalMatches[i];
			int otherPos = match->getPos();
			othersUsed[otherPos] = true;
			double score = match->getScore();
			WatershedInfo* watersh1 = watersheds[i];
			WatershedInfo* watersh2 = otherSeg->watersheds[otherPos];
//			*outputter << "Contour1 = " << i << "  and Contour2 = " << otherPos << " with score " << score;
//			outputter->writeLog();
			if (option == 'v') {
				cvZero(displayPic);
				cvDrawContours(displayPic, watersh1->getContour(), CV_RGB(255, 255, 0), CV_RGB(255, 255, 0), 0,2);
				cvDrawContours(displayPic, watersh2->getContour(), CV_RGB(0, 255, 255), CV_RGB(0, 255, 255), 0,2);
				cvShowImage("CONTOURS", displayPic);
				cvWaitKey(0);
			}
			int thisSegSize = watersh1->getSizeInPixels();
			int otherSegSize = watersh2->getSizeInPixels();
			thisCovered += thisSegSize;
			otherCovered += otherSegSize;
			double sizePerc = ((double) (thisSegSize + otherSegSize)) / (2 * imageArea);
			double thisScore = (score * sizePerc);
			totalScore += thisScore;
			*outputter << "Contour-match: " << thisScore << " from " << score << "  perc " << sizePerc;
			outputter->writeLog();
			countMatches++;
		}
		else {  // for contours from this segmentation that are unmatched... add up their pixel counts...
			thisNotCovered += watersheds[i]->getSizeInPixels();
		}
	}

	// This loop will count up the number of pixels from unmatched segments in the other segmentation
	for (int i = 0; i < otherSize; i++) {
		if ( !othersUsed[i] ) {
			otherNotCovered += otherSeg->watersheds[i]->getSizeInPixels();
		}
	}
	// compute total pixels covered and total not covered (Note that border regions won't be counted
	// either way, so these won't add up to 100%)
//	int totalCovered = thisCovered + otherCovered;
	int totalNotCovered = thisNotCovered + otherNotCovered;
//	double percCovered =  ((double)totalCovered) / (2 * imageArea);

	// Apply a penalty for the unmatched contours, scaling the penalty by the percentage of the total size that is unmatched
	double percNotCovered =  ((double)totalNotCovered) / (2 * imageArea);
	double penalty = 500.0 * percNotCovered;
	double finalScore = totalScore + penalty;

//	double perc1 = (double)countMatches / thisSize * 100.0;
//	double perc2 = (double)countMatches / otherSize * 100.0;

	//	*outputter << "Total score = " << totalScore << "  Penalty applied = " << penalty << " and final score = " << finalScore;
//	outputter->writeLog();
//	*outputter << countMatches << " out of " << thisSize << " contours were matched: " << perc1 << "%";
//	outputter->writeLog();
//	*outputter << countMatches << " out of " << otherSize << " second contours were matched: " << perc2 << "%";
//	outputter->writeLog();

	return finalScore;
}





void ImageSegmentation::computeWatershed() {
	// Set up temporary images
	IplImage* img_gray = cvCloneImage( image );
	IplImage* simplified = cvCloneImage( image );
	IplImage* nonCanny = cvCreateImage(cvGetSize( image ), 8, 1);
	IplImage* distMat = cvCreateImage(cvGetSize( image ), IPL_DEPTH_32F, 1);


	cvZero( wshedImage );

	cvMorphologyEx(image, simplified, NULL, element, CV_MOP_OPEN, 2);
	cvMorphologyEx(simplified, simplified, NULL, element, CV_MOP_CLOSE, 2);
	// Set up images
	cvCvtColor(simplified, cannyImage, CV_RGB2GRAY); // TODO:  changed this to skip simplified to see effect on moreCulledPics
	cvCvtColor( cannyImage, img_gray, CV_GRAY2BGR );

	// Compute Canny edges
	cvCanny(cannyImage, cannyImage, 10, 70, 3);
	cvNot(cannyImage, nonCanny);
	cvDistTransform(nonCanny, distMat, CV_DIST_L1, 3);
	cvConvert(distMat, startMarkers);
	cvThreshold(startMarkers, startMarkers, 15, 255, CV_THRESH_BINARY);


	// Create contours for generating the watershed
	findWatersheds();

	collectSignifWatersheds();
	// create watershed display image
	cvAddWeighted( wshedImage, 0.5, img_gray, 0.5, 0, wshedImage );

	// clear away temporary images
	cvReleaseImage(&img_gray);
	cvReleaseImage(&simplified);
	cvReleaseImage(&nonCanny);
	cvReleaseImage(&distMat);
}



	/*
	 * It computes contours for the cannyImage, then uses those contours to make a marker image that
	 * has the different contours marked with small integer values.  It then fills in the watersheds
	 * on that marker image.  Finally, it generates random colors to paint the watershed image, and paints
	 * each pixel belonging to a given watershed with the randomly chosen color.  It returns the number
	 * of watersheds created.
	 */
void ImageSegmentation::findWatersheds() {
		IplImage* markers = cvCreateImage( cvGetSize(image), IPL_DEPTH_32S, 1 );
		CvSeq* contours = NULL;

		cvZero( markers );

		cvFindContours( startMarkers, storage, &contours, sizeof(CvContour),
				CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE );

		int comp_count = 0;
		while (contours != NULL )
		{
			cvDrawContours( markers, contours, cvScalarAll(comp_count + 1),
					cvScalarAll(comp_count + 1), -1, -1, 8, cvPoint(0,0) );
			 contours = contours->h_next;
			 comp_count++;
		}
		foundWatersheds = comp_count;

		cvWatershed( image, markers );

		transferWatershed(markers);
		cvConvertScale(markers, visMarkers, 1, 0);

		cvReleaseImage(&markers);
	}


	/*
	 * Given the marker image with its watershed labels, transfer the data to the watershed image
	 * for display, using one of the table elements for it, and transfer the data also to
	 * the visible markers image, a displayable form.
	 */
void ImageSegmentation::transferWatershed(IplImage* markers) {

		// paint the watershed image
		for( int i = 0; i < markers->height; i++ ) {
			for( int j = 0; j < markers->width; j++ ) {
				int idx = CV_IMAGE_ELEM(markers, int, i, j);
				if( idx == -1 ) {
					cvSet2D(wshedImage, i, j, cvScalar(255, 255, 255));
				}
				else if( idx <= 0 || idx > color_tab->width ) {
					cvSet2D(wshedImage, i, j, cvScalar(0, 0, 0));
				}
				else {
					uchar* ptr = color_tab->data.ptr + (idx-1)*3;
					cvSet2D(wshedImage, i, j, cvScalar(ptr[0], ptr[1], ptr[2]));
				}
			}
		}
	}







void ImageSegmentation::collectSignifWatersheds() {

	IplImage* threshMarkers = cvCreateImage(cvGetSize(visMarkers), 8, 1);

	WatershedInfo* nextWatershed = NULL;

	for(int i = 1; i <= foundWatersheds;  i++) {
		cvThreshold(visMarkers, threshMarkers, i, -1, CV_THRESH_TOZERO_INV);
		cvThreshold(threshMarkers, threshMarkers, i-1, 255, CV_THRESH_BINARY);
		nextWatershed = new WatershedInfo(threshMarkers, outputter);
		if ( nextWatershed->isSignificant() ) {
			watersheds.push_back(nextWatershed);
		}
		else {
			delete nextWatershed;
		}
	}
}

void ImageSegmentation::foobarPrintPixels(IplImage* image) {
	for ( int r = 0; r < image->height; r++ ) {
		for ( int c = 0; c < image->height; c++ ) {
			unsigned char idx = CV_IMAGE_ELEM(image, unsigned char, r, c);
			cout << setfill(' ') << setw(4) << (unsigned short)idx;
		}
		cout << endl;
	}
}

/*
 * Given a color table matrix of the right size, it generates a random color for each
 * slot in the table.
 */
void ImageSegmentation::makeColorMapping(CvMat* color_tab) {
	// Create color table for displaying watersheds
	CvRNG rng = cvRNG(-1);

	for( int i = 0; i < color_tab->width; i++ )
            {
                uchar* ptr = color_tab->data.ptr + i*3;
                ptr[0] = (uchar)(cvRandInt(&rng)%180 + 50);
                ptr[1] = (uchar)(cvRandInt(&rng)%180 + 50);
                ptr[2] = (uchar)(cvRandInt(&rng)%180 + 50);
            }
}


int ImageSegmentation::scaleToColor(int value, int maxValue) {
		float perc = ((float) value) / maxValue;
		float newVal = perc * 200 + 55;
		int newColor = (int) cvRound(newVal);
		return newColor;
	}




