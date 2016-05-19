/*
 * CompareWatershed.cpp
 *
 *  Created on: Jul 5, 2011
 *      Author: susan
 *
 *  CompareWatershed is a subclass of the CompareInfo class, it contains the information needed to determine a match between
 *  a given segment in one image and a segment in another.  In this case, it inherits the similarityScore, and adds the
 *  position of the segment in the list of segments, and its WatershedInfo object.  This class is used to place in a priority queue;
 *  we use the queue to keep track of the best matches for each segment in an image, so that we can pair the segment up.
 */

#include "CompareWatershed.h"


/*
 * Constructor takes the watershed segment information, its position in the list of segments, and its similarity score
 * and it stores them away.
 */
CompareWatershed::CompareWatershed(WatershedInfo* info, int pos, double simScore) {
	comparedInfo = info;
	position = pos;
	similarityScore = simScore;
}


/*
 * Default destructor has nothing to do
 */
CompareWatershed::~CompareWatershed() {
}


/*
 * Accessor to return the watershed segment information
 */
WatershedInfo* CompareWatershed::getInfo() {
	return comparedInfo;
}


/*
 * Accessor to return the position
 */
int CompareWatershed::getPos() {
	return position;
}

