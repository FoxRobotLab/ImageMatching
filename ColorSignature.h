/*
 * ColorSignature.h
 *
 *  Created on: Jul 8, 2011
 *      Author: susan
 *
 *  The ColorSignature class represents a feature that represents the overall colors in the picture.  Rather than representing them
 *  as a histogram (another choice to be considered), this represents a matrix of color values, each one of which is the average
 *  of color values in a 20x20 pixel region of the original image.  These matrices can be compared together using Euclidean distance
 *  metrics
 */

#ifndef COLORSIGNATURE_H_
#define COLORSIGNATURE_H_


#include <cv.h>
#include <highgui.h>
#include "OutputLogger.h"



class ColorSignature {
protected:
	// The color signature, represented as rgb colors
	CvMat* rgbSignature;
	// The output logger, must be passed in to the constructor
	OutputLogger* outputter;
	// The area of the input image
	int imageArea;
	// The tilesize... currently fixed at 20
	int tileSize;

public:
	/* Constructor takes an image and a logger and builds the color signature */
	ColorSignature(IplImage* image, OutputLogger* logger);
	/* Destructor needs to clean up the CvMat signature */
	virtual ~ColorSignature();

	/* Takes a window name and a location and displays the feature, the signature matrix as an image */
	void displayFeaturePics(const char* windowName, int startX, int startY);

	/* Compares this color signature to an other color signature */
	double evaluateSimilarity(ColorSignature* otherSig);

private:
	/* Helper for the constructor to build the color signature */
	CvMat* extractColorSignature(IplImage* workingImage);
};



#endif /* COLORSIGNATURE_H_ */
