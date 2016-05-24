/*
 * HoughLines.h
 *
 *  Created on: Jun 8, 2011
 *      Author: susan
 *
 *  The HoughLines object contains information about lines found in the image using the cvCanny edge detection and
 *  Hough lines detection algorithms.  It copies the lines from the CvSeq format they are found in to a vector
 *  of vectors of CvPoints, for easier use.  It compares similarity between HoughLines objects by matching lines
 *  to each other, trying to maximize the similarity of matched lines.
 */

#ifndef HOUGHLINES_H_
#define HOUGHLINES_H_

#include <cv.h>
#include <highgui.h>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include "CompareHough.h"
#include "OutputLogger.h"

#define PI 3.14159265

using std::vector;
using std::string;
using std::endl;
using std::min;



class HoughLines {
protected:
	// The image that was passed in (not a copy)
	IplImage* image;
	// An output logger object, must be passed into the constructor
	OutputLogger* outputter;
	// A vector of lines, each line is a vector containing two points
	vector<vector<CvPoint> > lines;
	// The number of lines in the vector
	int numLines;


public:

	/*
	 * Constructor takes an IplImage * and generates the Hough Lines for it, using cvCanny and the probabilistic
	 * Hough Lines cvHoughLines
	 */
	HoughLines(IplImage* newImage, OutputLogger* op);


	/*
	 * Copy constructor that takes in one object and copies it to the new one
	 */
	HoughLines(HoughLines* other);


	/*
	 * Destructor does its thing
	 */
	~HoughLines();


	/*
	 * Takes in another HoughLines object and evaluates the similarity between the Hough Lines
	 */
	double evaluateSimilarity(HoughLines* other);

	/*
	 * Given the name of a window and where it should be placed, it creates and displays a window
	 * or windows showing this feature
	 */
	void displayFeaturePics(const char* windowName, int startX, int startY);


private:
	/*
	 * Works on the input image, and finds Hough lines in it.
	 * This is a helper for the constructor
	 */
	void findHoughLines();

	/**
	 * Sorts the points in lines so that they are ordered by their leftmost point
	 * Helper for findHoughLines
	 */
	void sortCvPoints();

	/*
	 * Compares 2 points and returns whether or not a point is the left-most point in a line
	 * Helper for sortCvPoints
	 */
	bool firstPointBefore(vector<CvPoint> p1, vector<CvPoint> p2 );


	/*
	 * A helper for evaluateSimilarity, it takes two lines and computes their similarity
	 */
	double compareLines(const vector<CvPoint> line1, const vector<CvPoint> line2);

	/*
	 * Given a line, it computes the angle of the line in degrees, in the range [0, 180)
	 */
	double calcAngle(const vector<CvPoint> line);

	/*
	 * Does greedy matching of lines from this to lines from the other, using the
	 * array of priority queues.
	 */
	void chooseFinalMatches(MatchPQ* matchQueues, CompareHough** finalMatches);

	/* Helper for chooseFinalMatches, it returns true if the line given by pos
	 * has already been placed in a match
	 */
	bool inUse(int pos, vector<int>inUse);

};


#endif /* HOUGHLINES_H_ */
