/*
 * Waterfall.h
 *
 *  Created on: Jul 22, 2011
 *      Author: macalester
 */

#ifndef WATERFALL_H_
#define WATERFALL_H_
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
#include "WeightedMatrixGraph.h"





class Waterfall {
protected:
	CvMat* color_tab;
	CvMemStorage* storage;
	IplConvKernel* element;
	OutputLogger* outputter;
	//various images needed
	IplImage* image;
	IplImage* cannyImage;
	IplImage* visMarkers;
	IplImage* markers;
	IplImage* wshedImage;
	IplImage* waterfallImage;
	IplImage* startMarkers;
	IplImage* img_gray;

	std:: vector<WatershedInfo*> watersheds;
	int imageHeight;
	int imageWidth;
	int imageArea;
	int foundWatersheds;

public:
	Waterfall(IplImage* img, OutputLogger* op);
	~Waterfall();
	double evaluateSimilarity(Waterfall* otherSeg, char option = ' ');
	IplImage* getWatershedPic();
	IplImage* getCannyPic();
	void foobarPrintPixels(IplImage* image);
	void displayFeaturePics(const char* windowName);


private:
	void computeWatershed();

	void findWatersheds();
	void transferWatershed(IplImage* markers);

	void collectSignifWatersheds();


	void chooseFinalMatches(MatchPQ* matchQueues, CompareWatershed** finalMatches);
	double computeTotalScore(CompareWatershed** finalMatches, Waterfall* otherSeg, char option, IplImage* displayPic);

	bool inUse(int pos, vector<int>inUse);
	void makeColorMapping(CvMat* color_tab);
	int scaleToColor(int value, int maxValue);

	void computeWaterfall();
	void mergeMarkerRegions(IplImage* markers, int newRegions[], int numRegions);

};

#endif /* WATERFALL_H_ */
