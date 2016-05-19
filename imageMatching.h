/*
 * imageMatching.h
 *
 *  Created on: Jun 8, 2011
 *      Author: susan
 *
 *  This program extracts features from images and compares the images for similarity based on those
 *  features.  See the README for more details.
 */

#ifndef IMAGEMATCHING_H_
#define IMAGEMATCHING_H_


// Include files
#include<iostream>
#include<iomanip>
#include<string>
#include<vector>
#include<stdlib.h>
#include "cv.h"
#include "highgui.h"
#include "ImageFeatures.h"
#include "OutputLogger.h"

using std::cin;
using std::cout;
using std::ios;
using std::setiosflags;
using std::setprecision;


// Forward declarations of functions in imageMatching.cpp

// makes a collection and compares other pictures as user picks
void makeCollection(OutputLogger* outputter);

//same as make collection, except it compares the pictures to the web camera
void makeCamera(OutputLogger* outputter);

// Asks user for two pictures and compares them, and repeats
void compareSelected(OutputLogger* outputter);

// Compares all pictures in a given folder to all pictures
void compareAllPictures(OutputLogger* outputter);


// The next three convert file numbers to names, and read in the images with those names
IplImage* getFileByNumber(int filenum, const char* directory);
void putFileByNumber(int filenum, IplImage* image);
const char* makeFilename(int fileNum, const char* directory);


// A few global variables
const char* currDirectory = "/Users/robot/Desktop/currPICS/";
const char* secondDirectory = "";
const char* newDirectory = "";
const char* currExtension = ".jpeg";
int startPicture = 0;
int numPictures = 142;

#endif /* IMAGEMATCHING_H_ */
