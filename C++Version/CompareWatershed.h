/*
 * CompareWatershed.h
 *
 *  Created on: Jul 5, 2011
 *      Author: susan
 *
 *  CompareWatershed is a subclass of the CompareInfo class, it contains the information needed to determine a match between
 *  a given segment in one image and a segment in another.  In this case, it inherits the similarityScore, and adds the
 *  position of the segment in the list of segments, and its WatershedInfo object.  This class is used to place in a priority queue;
 *  we use the queue to keep track of the best matches for each segment in an image, so that we can pair the segment up.
 *
 */

#ifndef COMPAREWATERSHED_H_
#define COMPAREWATERSHED_H_


#include "CompareInfo.h"
#include"WatershedInfo.h"




class CompareWatershed: public CompareInfo {
protected:
	// The information about a given watershed segment
	WatershedInfo* comparedInfo;
	// the position in the list of segments of this segment
	int position;

public:
	/* Constructor takes the watershed information, position, and score and stores them */
	CompareWatershed(WatershedInfo* info, int pos, double simScore);
	/* Default destructor has nothing to do */
	virtual ~CompareWatershed();

	/* Accessor to get the watershed information back out */
	WatershedInfo* getInfo();
	/* Accessor to get the position back out */
	int getPos();
};



#endif /* COMPAREWATERSHED_H_ */
