/*
 * ColorSignature.cpp
 *
 *  Created on: Jul 8, 2011
 *      Author: susan
 *
 *  The ColorSignature class represents a feature that represents the overall colors in the picture.  Rather than representing them
 *  as a histogram (another choice to be considered), this represents a matrix of color values, each one of which is the average
 *  of color values in a 20x20 pixel region of the original image.  These matrices can be compared together using Euclidean distance
 *  metrics
 */

#include "ColorSignature.h"


/*
 * Takes in an image and a logger and builds the color signature.  Also initializes other instance
 * variables that are needed
 */
ColorSignature::ColorSignature(IplImage* image, OutputLogger* logger) {
	outputter = logger;
	imageArea = image->width * image->height;
	tileSize = 20; // TODO: this should be a function of the width and height!
	rgbSignature = extractColorSignature(image);
}


/*
 * Releases the signature matrix
 */
ColorSignature::~ColorSignature() {
	cvReleaseMat(&rgbSignature);
}


/*
 * Given an image, this divides the image into 20x20 pixel sections.  For each section
 * It computes the average color and stores that into the signature matrix
 */
CvMat* ColorSignature::extractColorSignature(IplImage* workingImage) {
	int matWidth = workingImage->width / tileSize;
	int matHeight = workingImage->height / tileSize;
	CvMat* colorSig = cvCreateMat(matHeight, matWidth, CV_8UC3);
	CvScalar colorSlot;

	for (int tileRow = 0; tileRow < matHeight; tileRow++) {
		for (int tileCol = 0; tileCol < matWidth; tileCol++) {
			int imgRow = (tileRow * tileSize);
			int imgCol = (tileCol * tileSize);
			cvSetImageROI(workingImage, cvRect(imgCol, imgRow, tileSize, tileSize));
			colorSlot = cvAvg(workingImage);
			cvResetImageROI(workingImage);
			uchar* sigVals = (uchar*)CV_MAT_ELEM_PTR(*colorSig, tileRow, tileCol);
			sigVals[0] = colorSlot.val[0];
			sigVals[1] = colorSlot.val[1];
			sigVals[2] = colorSlot.val[2];
		}
	}
	return colorSig;
}


/*
 *  Given another color signature, evaluate the similarity.  This is done using the CvNorm
 *  function, which computes a variation on Euclidean distance, but relative to one or the
 *  other picture.  In this case, we compute both relative norms, and then add them up.
 *  The return value will be in the range from 0.0 to 200.0, unles the signatures can't be compared.
 */
double ColorSignature::evaluateSimilarity(ColorSignature* otherSig) {
	if ( (rgbSignature->rows != otherSig->rgbSignature->rows) || (rgbSignature->cols != otherSig->rgbSignature->cols) ) {
		return -1.0;
	}
	double relNorm1 = cvNorm(rgbSignature, otherSig->rgbSignature, CV_RELATIVE_L2);
	double relNorm2 = cvNorm(otherSig->rgbSignature, rgbSignature, CV_RELATIVE_L2);
	return (relNorm1 + relNorm2) * 100;

}



/*
 * Given a window name and a starting location on the screen, this creates an image
 * that represents the color signature and displays it.
 */
void ColorSignature::displayFeaturePics(const char* windowName, int startX, int startY) {
	IplImage* rgbSigImage;
	int dispWidth = rgbSignature->width * tileSize;
	int dispHeight = rgbSignature->height * tileSize;
	rgbSigImage = cvCreateImage( cvSize(dispWidth, dispHeight), 8, 3 );

	for (int row = 0; row < rgbSignature->height; row++) {
		int imageRow = row * tileSize;
		for (int col = 0; col < rgbSignature->width; col++) {
			int imageCol = col * tileSize;
			uchar* sigValsRGB = (uchar*)CV_MAT_ELEM_PTR(*rgbSignature, row, col);

			CvScalar color;
			color.val[0] = sigValsRGB[0];
			color.val[1] = sigValsRGB[1];
			color.val[2] = sigValsRGB[2];
			cvRectangle(rgbSigImage, cvPoint(imageCol, imageRow),
					cvPoint(imageCol + tileSize, imageRow + tileSize), color, CV_FILLED);
		}
	}
	cvNamedWindow(windowName);
	cvMoveWindow(windowName, startX, startY);
	cvShowImage(windowName, rgbSigImage);

	cvReleaseImage(&rgbSigImage);
}
