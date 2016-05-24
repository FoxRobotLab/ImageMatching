/*
 * ImageFeatures.h
 *
 *  Created on: Jun 8, 2011
 *      Author: susan
 *  Header file for the ImageFeatures class, which stores the different kinds of image features
 *  used for comparisons.  Information include the filename, the size of the image, plus a HoughLines object and an
 *  ImageSegmentation object.  Methods include creating with a text file, or from an IplImage object, accessors, and a
 *  comparison operation for evaluating similarity.
 */

#ifndef IMAGEFEATURES_H_
#define IMAGEFEATURES_H_

#include<iostream>
#include "cv.h"
#include "highgui.h"

#include "HoughLines.h"
#include "ImageSegmentation.h"
#include "ColorSignature.h"
#include "OutputLogger.h"

using std::string;


class ImageFeatures {

protected:
	// The image, which is copied from the one passed in
	IplImage* image;
	// The Hough lines feature
	HoughLines* lineInfo;
	// Image segmentation feature
	ImageSegmentation* segmentInfo;
	// Color signature feature
	ColorSignature* colorSignature;
	// Output logger, must be input to the constructor
	OutputLogger* outputter;


public:


	/*
	 * Constructor takes an IplImage pointer and constructs the image feature information from it.
	 */
	ImageFeatures(IplImage* picture, OutputLogger* op);


	/*
	 * Copy constructor that takes in one object and copies it to the new one
	 */
	ImageFeatures(ImageFeatures* other);

	/*
	 * Releases the image, if any, and deletes the lineInfo and segmentInfo data, if any
	 */
	~ImageFeatures();



	/* Given a window name and a location on the screen, this displays the features that result */
	void displayFeaturePics(const char* windowName, int startX, int startY);


	/*
	 * Similarity evaluation, takes another ImageFeatures object and evaluates similarity
	 */
	double evaluateSimilarity(ImageFeatures* otherFeatures, char option = ' ');
};

#endif /* IMAGEFEATURES_H_ */
