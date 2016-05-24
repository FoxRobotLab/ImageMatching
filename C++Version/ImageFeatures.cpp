/*
 * ImageFeatures.cpp
 *
 *  Created on: Jun 8, 2011
 *      Author: susan
 *
 *  Contains the definition of methods for the ImageFeatures class, which stores the different kinds of image features
 *  used for comparisons.  Information include the filename, the size of the image, plus a HoughLines object and an
 *  ImageSegmentation object.  Methods include creating with a text file, or from an IplImage object, accessors, and a
 *  comparison operation for evaluating similarity.
 */

#include "ImageFeatures.h"


/*
 * Constructor takes an IplImage pointer and constructs the image feature information from it.
 * Note that name is an optional parameter, defaults to "" if none is given
 */
ImageFeatures::ImageFeatures(IplImage* picture, OutputLogger* op) {
	image = cvCloneImage(picture);
	lineInfo = new HoughLines(image, op);
	segmentInfo = new ImageSegmentation(image, op);
	colorSignature = new ColorSignature(image, op);
	outputter = op;

}


/*
 * Copy constructor that takes in one object and copies it to the new one
 *
 */
ImageFeatures::ImageFeatures(ImageFeatures* other) {
	this->image = cvCloneImage(other->image);
	this->lineInfo = other->lineInfo;
	this->segmentInfo = other->segmentInfo;
	this->colorSignature = other->colorSignature;
	this->outputter = other->outputter;
}


/*
 * Releases the image, if any, and deletes the lineInfo and segmentInfo data, if any
 */
ImageFeatures::~ImageFeatures() {
	delete lineInfo;
	delete segmentInfo;
	delete colorSignature;
	cvReleaseImage(&image);
}





/*
 * Given a window name and a starting location on the screen, it has Hough lines, segmentation,
 * and color signature each display their results, starting at different locations
 */
void ImageFeatures::displayFeaturePics(const char* windowName, int startX, int startY) {
	string houghName = windowName;
	houghName.append("Hough-Lines");
	lineInfo->displayFeaturePics(houghName.c_str(), startX, startY);
	string segName = windowName;
	segName.append("Segmentation");
	segmentInfo->displayFeaturePics(segName.c_str(), startX, startY + 300);
	string name1 = windowName;
	name1.append("ColorSig-");
	colorSignature->displayFeaturePics(name1.c_str(), startX, startY+600);
}



/*
 * Similarity evaluation, takes another ImageFeatures object and evaluates similarity
 * It computes the similarity based on Hough lines, based on image segmentation, and based
 * on color signature.  The image segmentation is capped at 2000.0, and the hough similarity
 * tends to be half the size of the others, so it is doubled, and the three values are summed
 * to compute the final similarity
 */
double ImageFeatures::evaluateSimilarity(ImageFeatures* otherFeatures, char option) {
	*outputter << "*********************************************";
	outputter->writeLog();
	double houghSim = lineInfo->evaluateSimilarity(otherFeatures->lineInfo);
	*outputter << "Hough sim = " << houghSim;
	outputter->writeLog();
	double segmentSim = segmentInfo->evaluateSimilarity(otherFeatures->segmentInfo, option);
	*outputter << "Segment sim = " << segmentSim;
	outputter->writeLog();
	segmentSim = min(2000.0, segmentSim);
	double colorsigSim = colorSignature->evaluateSimilarity(otherFeatures->colorSignature);
	*outputter << "ColorSig sim = " << colorsigSim;
	outputter->writeLog();
	*outputter << "Final similarity score = " << (segmentSim + colorsigSim + 2 * houghSim);
	outputter->writeLog();
	*outputter << "*********************************************";
	outputter->writeLog();
	return segmentSim + colorsigSim + 2 * houghSim;
}



