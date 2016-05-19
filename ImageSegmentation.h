/*
 * ImageSegmentation.h
 *
 *  Created on: Jun 8, 2011
 *      Author: susan
 */

#ifndef IMAGESEGMENTATION_H_
#define IMAGESEGMENTATION_H_

#include "cv.h"
#include "highgui.h"
#include <stdio.h>
#include <stdlib.h>
#include<iostream>
#include <fstream>
#include<vector>
#include<queue>

#include "WatershedInfo.h"
#include "OutputLogger.h"
#include "CompareWatershed.h"


using std::setfill;
using std::setw;



class ImageSegmentation {
protected:
	CvMat* color_tab;
	CvMemStorage* storage;
	IplConvKernel* element;
	OutputLogger* outputter;

	IplImage* image;
	IplImage* cannyImage;
	IplImage* visMarkers;
	IplImage* wshedImage;
	IplImage* startMarkers;

	vector<WatershedInfo*> watersheds;
	int imageHeight;
	int imageWidth;
	int imageArea;
	int foundWatersheds;

public:
	ImageSegmentation(IplImage* img, OutputLogger* op);
	~ImageSegmentation();
	double evaluateSimilarity(ImageSegmentation* otherSeg, char option = ' ');
	IplImage* getWatershedPic();
	IplImage* getCannyPic();
	void foobarPrintPixels(IplImage* image);
	void displayFeaturePics(const char* windowName, int startX, int startY);


private:
	void computeWatershed();

	void findWatersheds();
	void transferWatershed(IplImage* markers);

	void collectSignifWatersheds();


	void chooseFinalMatches(MatchPQ* matchQueues, CompareWatershed** finalMatches);
	double computeTotalScore(CompareWatershed** finalMatches, ImageSegmentation* otherSeg, char option, IplImage* displayPic);

	bool inUse(int pos, vector<int>inUse);
	void makeColorMapping(CvMat* color_tab);
	int scaleToColor(int value, int maxValue);


};


#endif /* IMAGESEGMENTATION_H_ */
