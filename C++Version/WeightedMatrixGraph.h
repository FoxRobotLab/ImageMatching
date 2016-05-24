/*
 * WeightedMatrixGraph.h
 *
 *  Created on: Jun 30, 2011
 *      Author: macalester
 */

#ifndef WEIGHTEDMATRIXGRAPH_H_
#define WEIGHTEDMATRIXGRAPH_H_
#include "Graph.h"
#include <vector>
#include <iostream>
#include <iomanip>
#include "Edge.h"


using std::cout;
using std::endl;
using std::setw;
using std::vector;

class WeightedMatrixGraph : public Graph {

protected:
double** adjMatrix;
int numNodes;
int numEdges; //number of edges in graph
int numTreeEdges; //no of edges in the tree
int sourceNode; //source node


public:
	WeightedMatrixGraph(int n);
	~WeightedMatrixGraph();
	bool addEdge(int nodeIndex1, int nodeIndex2, double weight);
	std::vector<int> getNeighbors(int nodeIndex);
	double getWeight(int nodeIndex1, int nodeIndex2);
	std::vector<int> BFSRoute(WeightedMatrixGraph* graph, int startVert, int goalVert);
	std::vector<int> reconstructPath(int startVert, int goalVert, std::vector<int> preds);
	WeightedMatrixGraph* findMST(int sourceNode, int* skipSet, int numToSkip);
	void printMatrix();
	int* findRme();

private:
	int* mergeRegions(vector<vector<Edge> > minimumEdges);
	bool isInSet(int x, int numInSet, int* nodeSet);
	bool isInSet(int x, vector<int> nodeSet);
	bool isInSet(int x, vector<Edge>edgeSet);
};

#endif /* WEIGHTEDMATRIXGRAPH_H_ */
