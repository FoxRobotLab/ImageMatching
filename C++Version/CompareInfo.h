/*
 * CompareInfo.h
 *
 *  Created on: Jul 5, 2011
 *      Author: susan
 *
 *  This file contains the definition of the CompareInfo class, the myCompare class, and a type definition for the
 *  particular kind of priority queue we are using.  The priority queue called MatchPQ contains objects of CompareInfo
 *  type, and it has a "comparer" object to do the actual comparisons.  That is a myCompare object, which depends on
 *  the objects having a compareTo method.
 *
 */

#ifndef COMPAREINFO_H_
#define COMPAREINFO_H_

#include<queue>

using std::vector;
using std::priority_queue;


/*
 *  The CompareInfo class is a parent class that should really be replaced by its kids.
 *  It is designed to be used in the priority queue to store whatever information about a match is pertinent
 *  besides the similarity score, so we can later determine which image feature matched.  This class should
 *  only be used on its own when testing the priority queue
 */
class CompareInfo {
protected:
	// The similarity score used by the priority queue to order these objects
	double similarityScore;

public:
	/* Default constructor, needed by subclasses */
	CompareInfo();

	/* Constructor that takes a score, for testing purposes only */
	CompareInfo(double simScore);
	/* Default destructor has nothing to do */
	virtual ~CompareInfo();

	/* Comparison method needed below in myCompare for the priority queue*/
	bool compareTo(const CompareInfo* otherCI);
	/* Accessor for the similarity score */
	double getScore();
};


/*
 * A class intended to compare two CompareInfo objects together, needed by the priority queue class
 * in the standard library
 */
class myCompare {
public:
	/* Default constructor */
	myCompare() {}

	/* This object overloads a function call operation, used by the priority queue class.  you "call"
	 * the myCompare object and pass it two CompareInfo objects, and it calls this method and returns
	 * the result of comparing the two, using thier compareTo methods
	 */
	bool operator() ( CompareInfo* lhs, const  CompareInfo* rhs) const {
		return lhs->compareTo(rhs);
	}
};


/* This defines the particular type of priority queue we will be using */
typedef priority_queue<CompareInfo*, vector<CompareInfo*>, myCompare> MatchPQ;



#endif /* COMPAREINFO_H_ */
