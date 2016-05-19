/*
 * Graph.cpp
 *
 *  Created on: Jun 28, 2011
 *      Author: macalester
 */
#include"Graph.h"


Graph::Graph() {
	numVerts = 10;
	std::vector<int> nodeData;
	lastNode = 0;
}

//Constructs a graph with n vertices and which contains the data stored in nodeData.
//lastNode will be equal to the number of elements in
Graph::Graph(int n, std::vector<int> nodeData) {
	this->numVerts = n;
	this->nodeData = nodeData;
	this->lastNode = this->nodeData.size();
}

int Graph::AddNodeData(int nodeData) {
	if (this->lastNode == this->numVerts) { //if lastNode equals numVerts, that means the graph is full
		std::cout << "Error: Graph is full!" << std::endl;
	}
	else { //otherwise, add the data to nodeData and increase lastNode
		this->nodeData.push_back(nodeData);
		int nodePos = this->lastNode;
		this->lastNode++;
		return nodePos;
	}
	return 0;
}

int Graph::getSize() {
	return numVerts;
}

int Graph::getData(int nodePos) { //gets the data at a certain position
	if (nodePos < numVerts && nodePos < lastNode) {
		return nodeData[nodePos];
	}
	else {
		std::cout << "No data here!" << std::endl;
		return 0;
	}
}

