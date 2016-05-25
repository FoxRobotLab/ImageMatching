""" ========================================================================
CompareInfo.py

Created: March and May, 2016
Author: Susan

This file contains the definition of the CompareInfo class, the myCompare
class, and a type definition for the particular kind of priority queue we are
using. The priority queue called MatchPQ contains objects of CompareInfo
type, and it has a "comparer" object to do the actual comparisons. That is a
myCompare object, which depends on the objects having a compareTo method.

This is porting the CompareInfo class written in C++ in about 2011.
======================================================================== """


import FoxQueue


class CompareInfo:
    """The CompareInfo class is a parent class that should really be replaced by
    its kids. It is designed to be used in the priority queue to store
    whatever information about a match is pertinent besides the similarity
    score, so we can later determine which image feature matched. This class
    should only be used on its own when testing the priority queue."""
 
    def __init__(self, givenScore = 0.0):
        """Default constructor, needed by subclasses."""
        self.similarityScore = givenScore
    
    
    def compareTo(self, otherCI):
        """Comparison method needed to compare two in priority queue..."""
        return self.similarityScore > otherCI.similarityScore
    
    def getScore(self):
        """Access that reports the similarity score that has been computed."""
        return self.similarityScore
    

def CompareFn(
def MatchPQ():
    