/*
 * WatershedInfo.h
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

#ifndef WATERSHEDINFO_H_
#define WATERSHEDINFO_H_


#include "cv.h"
#include "highgui.h"
#include<iostream>
#include<iomanip>
#include<stdarg.h>

#include "OutputLogger.h"

using std::endl;
using std::cout;


class WatershedInfo {
protected:
	// The output logger, must be provided in the constructor
	OutputLogger* outputter;
	// A chunk of memory used by some operations in openCV
	CvMemStorage* storage;
	// The image that contains the points belonging to the segment
	IplImage* watershedImage;
	// A sequence of points that bound the segment.
	CvSeq* contour;
	// The moments of the segment... we only use spatial and cewntra
	CvMoments moments;
	// The bounding rectangle of the segment
	CvRect boundingRect;
	// How many pixels in common must there be to consider two segments similar enough?
	double pixelThreshold;
	// How much of the bounding rectangles must overlap to consider two segments similar enough?
	double bboxThreshold;
	// How many pixels make up the segment
	int sizeInPixels;
	// Is the segment big enough to be considered significant?
	bool bigEnough;


public:
	/* Given the image containing just this segment and the logger, set up the information */
	WatershedInfo(IplImage* watershedImage, OutputLogger* op);

	/* Default destructor cleans up anything created, like the contour and storage */
	~WatershedInfo();

	/* Return the contour around this segment */
	CvSeq* getContour();
	/* Return a specific moment */
	double getMomentAt(int x, int y);
	/* Return the size of the segment in pixels */
	int getSizeInPixels();

	/* Returns true if the segment is big enough, and false otherwise */
	bool isSignificant();
	/* Compare another segment to this one for similarity */
	double evaluateSimilarity(WatershedInfo* otherInfo);

private:

	/* A helper for evaluateSimiliarty, this function takes two bounding rectangles and determines
	 * how much they overlap, and if they overlap enough
	 */
	bool rectanglesOverlap(CvRect rect1, CvRect rect2);

	/* A helper for rectanglesOverlap, it computes the amount of overlap in a given direction, given two values and
	 * the widths of each rectangles
	 */
	int overlapInDir(int val1, int val2, int range1, int range2);
//	void foobarPrintPixels();
};

#endif /* WATERSHEDINFO_H_ */
