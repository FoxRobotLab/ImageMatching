/*
 * CompareInfo.cpp
 *
 *  Created on: Jul 5, 2011
 *      Author: susan
 *
 *  The CompareInfo class is essentially an abstract class.  It is designed to be used in the priority queue
 *  to store whatever information about a match is pertinent besides the similarity score, so we can later determine
 *  which image feature matched.  This class should only be used on its own when testing the priority queue
 */

#include "CompareInfo.h"

/*
 * Default constructor needed by subclasses
 */
CompareInfo::CompareInfo() {
	similarityScore = 0.0;
}

/*
 * Should only use this method for testing purposes, it takes just a similarity score and stores it
 */
CompareInfo::CompareInfo(double simScore) {
	similarityScore = simScore;
}


/*
 * Destructor has nothing to do
 */
CompareInfo::~CompareInfo() {
}


/*
 * The crucial method, used to compare entries in the priority queue
 */
bool CompareInfo::compareTo(const CompareInfo* otherCI) {
	return (similarityScore > otherCI->similarityScore);
}


/*
 * Accessor for the similarity score
 */
double CompareInfo::getScore() {
	return similarityScore;
}

