/*
 * CompareWatershed.cpp
 *
 *  Created on: Jul 5, 2011
 *      Author: susan
 *
 *  CompareHough is a subclass of the CompareInfo class, it contains the information needed to determine a match between
 *  a Hough line in one image and a Hough line in another.  In this case, it inherits the similarityScore, and adds the
 *  position in the list of Hough lines of the one we are matching with.  This class is used to place in a priority queue;
 *  we use the queue to keep track of the best matches for each line in an image, so that we can pair the lines up
 */

#include "CompareHough.h"


/*
 * Constructor takes a position and the similarity score, and stores them in the object.
 */
CompareHough::CompareHough(int pos, double simScore) {
	position = pos;
	similarityScore = simScore;
}

/*
 * Default descructor has nothing to do
 */
CompareHough::~CompareHough() {
}


/*
 * An accessor to get the position out when it is needed
 */

int CompareHough::getPos() {
	return position;
}


