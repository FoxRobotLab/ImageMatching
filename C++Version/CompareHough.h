/*
 * CompareHough.h
 *
 *  Created on: Jul 5, 2011
 *  Author: susan
 *  Header file for the CompareHough class
 *
 *  CompareHough is a subclass of the CompareInfo class, it contains the information needed to determine a match between
 *  a Hough line in one image and a Hough line in another.  In this case, it inherits the similarityScore, and adds the
 *  position in the list of Hough lines of the one we are matching with.  This class is used to place in a priority queue;
 *  we use the queue to keep track of the best matches for each line in an image, so that we can pair the lines up
 */

#ifndef COMPAREHOUGH_H_
#define COMPAREHOUGH_H_


#include "CompareInfo.h"






class CompareHough: public CompareInfo {
protected:
	// The position in a list of lines of the particular line this object pertains to
	int position;

public:
	/* Constructor stores the position and the similarity score */
	CompareHough(int pos, double simScore);
	/* Destructor has nothing to do */
	virtual ~CompareHough();
	/* Access for retrieving the position (accessor for score is inherited */
	int getPos();

};





#endif /* COMPAREHOUGH_H_ */
