/*
 * Graph.h
 *
 *  Created on: Jun 28, 2011
 *      Author: macalester
 */

#ifndef GRAPH_H_
#define GRAPH_H_

#include<iostream>
#include<vector>

class Graph {
protected:
	int numVerts;
	std::vector<int> nodeData;
	int lastNode;

public:
	Graph();
	Graph(int n, std::vector<int> nodeData);
	int AddNodeData(int nodeData);
	int getSize();
	int getData(int nodePos);
private:
	int main();
};

#endif /* GRAPH_H_ */
