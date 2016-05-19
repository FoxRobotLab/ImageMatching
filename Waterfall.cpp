/*
 * Waterfall.cpp
 *
 *  Created on: Jul 22, 2011
 *  Author: Susan, Jackson and Scott
 *
 * This class contains the methods for computing a Waterfall, which is a modification to the Watershed algorithm in
 * OpenCV. The goal is to cut down on the number of regions that will need to be compared.
 * This is accomplished by merging regions with similar color values from the original picture.
 * The WeightedMatrixGraph data structure is crucial for Waterfall to complete.
 *
 * NOTE: Some of the methods are from ImageSegmentation for comments on how those function please refer to
 * ImageSegmentation
 */

#include "Waterfall.h"



Waterfall::Waterfall(IplImage* img, OutputLogger* op) {
	outputter = op;
	image = cvCloneImage(img);
	imageHeight = image->height;
	imageWidth = image->width;
	imageArea = imageHeight * imageWidth;
	foundWatersheds = 0;


	// Instance variables
	wshedImage = cvCloneImage(image);
	waterfallImage = cvCloneImage( image );
	cannyImage = cvCreateImage( cvGetSize(image), 8, 1 );
	visMarkers = cvCreateImage( cvGetSize(image), 8, 1);
	startMarkers = cvCreateImage(cvGetSize( image ), 8, 1);
	markers = cvCreateImage( cvGetSize(image), IPL_DEPTH_32S, 1 );
	// Initialize color table and storage
	color_tab = cvCreateMat( 1, 500, CV_8UC3 );
	makeColorMapping(color_tab);
	storage = cvCreateMemStorage(0);
	element = cvCreateStructuringElementEx(2, 2, 0, 0, CV_SHAPE_ELLIPSE);

	computeWatershed();
	computeWaterfall();
}

/**
 * Deconstructor
 */
Waterfall::~Waterfall() {
	//release the images
	cvReleaseImage(&cannyImage);
	cvReleaseImage(&waterfallImage);
	cvReleaseImage(&visMarkers);
	cvReleaseImage(&startMarkers);
	cvReleaseImage(&markers);
	cvReleaseImage(&wshedImage);
	cvReleaseImage(&img_gray);
	cvReleaseImage(&image);

	cvReleaseMat(&color_tab);
	cvReleaseMemStorage( &storage );
	cvReleaseStructuringElement(&element);

	// clear up the vector of watershed objects
	for (unsigned int i = 0; i < watersheds.size(); i++) {
		delete watersheds[i];
	}
	watersheds.clear();
}


double Waterfall::evaluateSimilarity(Waterfall* otherSeg, char option) {
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
		displayPic = cvCreateImage(cvGetSize(waterfallImage), 8, 3);
	}
	// loop through every contour in this segmentation.  For each contour,
	// consider every contour in otherSeg.  If that other contour matches this one
	// add it to this contour's priority queue, ordered in increasing order by similarity score
	for( int cnt1 = 0; cnt1 < thisSize; cnt1++ ) {
		ourNextInfo = watersheds[cnt1];
		for ( int cnt2 = 0; cnt2 < otherSize; cnt2++ ) {
			otherNextInfo = otherSeg->watersheds[cnt2];
			double nextScore = ourNextInfo->evaluateSimilarity(otherNextInfo);
			if ( (nextScore >= 0.0) ) {
				CompareWatershed* matchInfo = new CompareWatershed(otherNextInfo, cnt2, nextScore);
				matchQueues[cnt1].push(matchInfo);
//				*outputter << "Possible match " << cnt1 << " with " << cnt2 << " with score " << nextScore;
//				outputter->writeLog();
			}
		}  // for cnt2
 	} // for cnt1

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



IplImage* Waterfall::getWatershedPic() {
	return waterfallImage;
}

IplImage* Waterfall::getCannyPic() {
	return cannyImage;
}

void Waterfall::displayFeaturePics(const char* windowName) {
	cvShowImage(windowName, waterfallImage);
}


/* =============================================================================================
 * Private methods after this point
 * =============================================================================================
 */

void Waterfall::chooseFinalMatches(MatchPQ* matchQueues, CompareWatershed** finalMatches) {
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


bool Waterfall::inUse(int pos, vector<int>inUse) {
	for ( unsigned int i = 0; i < inUse.size(); i++ ) {
		if ( inUse[i] == pos ) {
			return true;
		}
	}
	return false;
}



double Waterfall::computeTotalScore(CompareWatershed** finalMatches, Waterfall* otherSeg, char option, IplImage* displayPic) {
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

	*outputter << "Total score = " << totalScore << "  Penalty applied = " << penalty << " and final score = " << finalScore;
	outputter->writeLog();
//	*outputter << "Percent covered by matched segments = " << percCovered << "   Percent not covered = " << percNotCovered;
//	outputter->writeLog();
	double perc1 = (double)countMatches / thisSize * 100.0;
	double perc2 = (double)countMatches / otherSize * 100.0;
	*outputter << countMatches << " out of " << thisSize << " contours were matched: " << perc1 << "%";
	outputter->writeLog();
	*outputter << countMatches << " out of " << otherSize << " second contours were matched: " << perc2 << "%";
	outputter->writeLog();


	return finalScore;
}





void Waterfall::computeWatershed() {
	// Set up temporary images
	img_gray = cvCloneImage( image );
	IplImage* simplified = cvCloneImage( image );
	IplImage* nonCanny = cvCreateImage(cvGetSize( image ), 8, 1);
	IplImage* distMat = cvCreateImage(cvGetSize( image ), IPL_DEPTH_32F, 1);


	cvZero( waterfallImage );

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

//	collectSignifWatersheds();
	// create watershed display image
	cvAddWeighted( waterfallImage, 0.5, img_gray, 0.5, 0, waterfallImage );

	// clear away temporary images
//	cvReleaseImage(&img_gray);
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
void Waterfall::findWatersheds() {
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
//		cvConvertScale(markers, visMarkers, 1, 0);
//		for(int i = 0; i< visMarkers -> height; i++){
//			for(int q = 0; q< visMarkers-> width; q++){
//				int pix = CV_IMAGE_ELEM(visMarkers, int, i, q);
//				cout << pix << " ";
//			}
//			cout << endl;
//		}

	}


/*
 * Given the marker image with its watershed labels, transfer the data to the watershed image
 * for display, using one of the table elements for it, and transfer the data also to
 * the visible markers image, a displayable form.
 */
void Waterfall::transferWatershed(IplImage* markers) {
		cvZero(visMarkers);

		// paint the watershed image
		for( int i = 0; i < markers->height; i++ ) {
			for( int j = 0; j < markers->width; j++ ) {
				int idx = CV_IMAGE_ELEM(markers, int, i, j);
				if( idx == -1 ) {
					cvSet2D(waterfallImage, i, j, cvScalar(255, 255, 255));
				}
				else if( idx <= 0 || idx > color_tab->width ) {
					cvSet2D(waterfallImage, i, j, cvScalar(0, 0, 0));
				}
				else {
					uchar* ptr = color_tab->data.ptr + (idx-1)*3;
					cvSet2D(waterfallImage, i, j, cvScalar(ptr[0], ptr[1], ptr[2]));
				}
			}
		}
	}





/**
 * looks for watersheds within a certain threshold and stores them
 */
void Waterfall::collectSignifWatersheds() {

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

void Waterfall::foobarPrintPixels(IplImage* image) {
	for ( int r = 0; r < image->height; r++ ) {
		for ( int c = 0; c < image->height; c++ ) {
			unsigned char idx = CV_IMAGE_ELEM(image, unsigned char, r, c);
			std::cout << std::setfill(' ') << std::setw(4) << (unsigned short)idx;
		}
		std::cout << std::endl;
	}
}

/*
 * Given a color table matrix of the right size, it generates a random color for each
 * slot in the table.
 */
void Waterfall::makeColorMapping(CvMat* color_tab) {
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


int Waterfall::scaleToColor(int value, int maxValue) {
		float perc = ((float) value) / maxValue;
		float newVal = perc * 200 + 55;
		int newColor = (int) cvRound(newVal);
		return newColor;
	}

/**
 * computeWaterfall: this is the main function for computing a waterfall
 */
void Waterfall::computeWaterfall(){
	//the graph which will store all the information
	WeightedMatrixGraph* g = new WeightedMatrixGraph(foundWatersheds+1);

	//scan through picture taking the lowest regional differences and storing them inside g
	for(int i = 1; i< markers -> height-1; i++){
		for(int q = 1; q< markers-> width-1; q++){
			//check left-right adjacency
			int pixRight = CV_IMAGE_ELEM(markers, int, i, q+1);
			int pixLeft = CV_IMAGE_ELEM(markers, int, i, q-1);
			if( (pixRight > 0) && (pixLeft > 0) && (pixLeft != pixRight) ) {
				//calculate color difference and set color difference table
				unsigned char* p1 = &CV_IMAGE_ELEM( image, unsigned char, i, q-1 );
				unsigned char* p2 = &CV_IMAGE_ELEM( image, unsigned char, i, q+1 );
				double dif = sqrt(pow((double)p1[0]-(double)p2[0], 2) + pow((double)p1[1]-(double)p2[1], 2) +pow((double)p1[2]-(double)p2[2], 2));
				if(dif < g -> getWeight(pixLeft,pixRight) || dif < g -> getWeight(pixLeft,pixRight) || g -> getWeight(pixLeft,pixRight) <= -1 || g -> getWeight(pixLeft,pixRight) <= -1){
					//add to graph
					g->addEdge(pixLeft, pixRight, dif);
				}
			}
			//check top-down adjacency
			int pixAbove = CV_IMAGE_ELEM(markers, int, i-1, q);
			int pixBelow = CV_IMAGE_ELEM(markers, int, i+1, q);
			if( (pixBelow > 0) && (pixAbove > 0) && (pixBelow!= pixAbove) ){
				//calculate color difference and set color difference table
				unsigned char* p1 = &CV_IMAGE_ELEM( image, unsigned char, i-1, q );
				unsigned char* p2 = &CV_IMAGE_ELEM( image, unsigned char, i+1, q );
				double dif = sqrt(pow((double)p1[0]-(double)p2[0], 2) + pow((double)p1[1]-(double)p2[1], 2) + pow((double)p1[2]-(double)p2[2], 2));
				if(dif < g -> getWeight(pixBelow,pixAbove) || dif < g -> getWeight(pixAbove,pixBelow) || g -> getWeight(pixBelow,pixAbove) <= -1 || g -> getWeight(pixAbove,pixBelow) <= -1){
					//add to graph
					g->addEdge(pixBelow, pixAbove, dif);
				}
			}
			//check right-left diagonal adjacency
			int pixBR = CV_IMAGE_ELEM(markers, int , i+1, q+1);
			int pixAL = CV_IMAGE_ELEM(markers, int , i-1, q-1);
			if( (pixBR > 0) && (pixAL > 0) && (pixBR != pixAL) ){
				//calculate color difference and set color difference table
				unsigned char* p1 = &CV_IMAGE_ELEM( image, unsigned char, i+1, q+1 );
				unsigned char* p2 = &CV_IMAGE_ELEM( image, unsigned char, i-1, q-1 );
				double dif = sqrt(pow((double)p1[0]-(int)p2[0], 2) + pow((double)p1[1]-(int)p2[1], 2) + pow((double)p1[2]-(double)p2[2], 2));
				if(dif < g -> getWeight(pixBR,pixAL) || dif < g -> getWeight(pixBR,pixAL) || g -> getWeight(pixBR,pixAL) <= -1 || g -> getWeight(pixAL,pixBR) <= -1){
					//add to graph
					g->addEdge(pixBR, pixAL, dif);
				}
			}
			//check left-right diagonal adjacency
			int pixBL = CV_IMAGE_ELEM(markers, int , i+1, q-1);
			int pixAR = CV_IMAGE_ELEM(markers, int , i-1, q+1);
			if( (pixBL > 0) && (pixAR > 0) && (pixAR != pixBL) ) {
				//calculate color difference and set color difference table
				unsigned char* p1 = &CV_IMAGE_ELEM( image, unsigned char, i-1, q+1 );
				unsigned char* p2 = &CV_IMAGE_ELEM( image, unsigned char, i+1, q-1 );
				double dif = sqrt(pow((double)p1[0]-(double)p2[0], 2) + pow((double)p1[1]-(double)p2[1], 2) + pow((double)p1[2]-(double)p2[2], 2));
				if(dif < g -> getWeight(pixAR,pixBL) || dif < g -> getWeight(pixBL,pixAR) || g -> getWeight(pixAR,pixBL) <= -1 || g -> getWeight(pixBL,pixAR) <= -1){
					//add to graph
					g->addEdge(pixAR, pixBL, dif);
				}
			}
		}
	}

	//find the MST
	int skipNodes[1] = {0};
	WeightedMatrixGraph* mst = g->findMST(1, skipNodes, 1);

	//find the regional minimums and merge them
	int* newRegions = mst ->findRme();
	mergeMarkerRegions(markers, newRegions, foundWatersheds);
	transferWatershed(markers);
	//blend the new waterfall and its original iamge so that you can see what's going on
	cvAddWeighted( waterfallImage, 0.5, img_gray, 0.5, 0, waterfallImage );

	//store information in visMarkers for later use
	cvConvertScale(markers, visMarkers, 1, 0);

	collectSignifWatersheds();

}

/**
 * mergeMarkerRegion: merges the marker regions returned by the Watershed algorithm
 * This is called after findRme()
 */
void Waterfall::mergeMarkerRegions(IplImage* markers, int newRegions[], int numRegions){
	//iterate through all of the regions
	for(int i = 1; i < numRegions; i++){
		if(newRegions[i] != i){	//if the ith region isn't equal to i then it was merged
			for(int q = 0; q < markers -> height; q++){
				for(int u = 0; u < markers -> width; u++){
					int pix = CV_IMAGE_ELEM( markers, int, q, u );
					if(pix == i){
						CvScalar s;
						s = cvGet2D(markers,q,u);
						s.val[0] = newRegions[i];
						s.val[1] = newRegions[i];
						s.val[2] = newRegions[i];
						s.val[3] = newRegions[i];
						//use s.val[0] up till [3] where it is ordered in terms of intensity, red, green, blue
						cvSet2D(markers,q,u,s);
					}
				}
			}
		}
	}
	//modify markers for region merging
	for(int i = 1; i < markers -> height-1; i++){
		for(int q = 1; q < markers -> width-1; q++){
			int pix = CV_IMAGE_ELEM(markers, int, i, q);
			if(pix == -1){	//check if the regions are the same across -1. if they are merge
				std::vector<int>toChange;	//stores the regions that will be merged
				for(int z = -1; z < 2; z++){	//scans 8 points around the image
					int pix1 = CV_IMAGE_ELEM(markers, int, i+z, q+z);
					for(int u = -1; u < 2; u++){
						int pix2 = CV_IMAGE_ELEM(markers, int, i+u, q+u);
						if(pix1 == pix2){	//if the regions are the same across border then merge
							toChange.push_back(pix1);
						}
						if(toChange.size() > 2){	//there are too many surrounding similar points to properly merge
							break;
						}
					}
				}
				//set up a scalar and save that scalar into markers
				CvScalar s;
				s = cvGet2D(markers, i,q);
				s.val[0] = toChange.at(0);
				s.val[1] = toChange.at(0);
				s.val[2] = toChange.at(0);
				s.val[3] = toChange.at(0);
				cvSet2D(markers,i,q,s);
			}

		}
	}
}



