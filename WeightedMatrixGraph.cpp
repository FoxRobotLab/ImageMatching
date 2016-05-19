/*
 * WeightedMatrixGraph.cpp
 *
 *  Created on: Jun 30, 2011
 *      Author: macalester
 * This class is a data structure for storing the information produced by a Watershed.
 * In addition, this class contains a method for computing a minimum spanning tree as well as
 * methods to compute the regional minimum edges of an minimum spanning tree. Other methods
 * include the ability to add edges etc. The graph is represented using an adjacency matrix.
 *
 */
#include "WeightedMatrixGraph.h"

/*
 * takes an integer, which is the number of vertices in the graph.  It creates the
 * adjacency matrix dynamically, and sets all the values in it to be -1, which means
 * there is no edge listed there.
 */
WeightedMatrixGraph::WeightedMatrixGraph(int n) {
	numNodes = n;
	//g = new Graph(numVerts, data);
	adjMatrix = new double*[numNodes];
	for (int i = 0; i < numNodes; i++) {
		adjMatrix[i] = new double[numNodes];
		for (int j = 0; j < numNodes; j++) {
			adjMatrix[i][j] = -1;
		}
	}
}

/*
 * Deletes the dynamically-created adjacency matrix.
 */
WeightedMatrixGraph::~WeightedMatrixGraph(){
    for(int i = 0; i < numNodes; i++){
        delete [] adjMatrix[i];
    }
    delete [] adjMatrix;
}

/* takes two indices and a weight value, and adds an edge between the two vertices
 * with the given weight.  It generates an error if the weight value is negative,
 * or if one or the other index is not in range.  Note: if an edge already existed
 * between these two, it updates the weight on the edge to be the new weight value
 */
bool WeightedMatrixGraph::addEdge(int nodeIndex1, int nodeIndex2, double weight) { //adds a weighted edge at a given location
	if ( (weight >= 0) && (nodeIndex1 < numNodes) && (nodeIndex2 < numNodes) ) {
		adjMatrix[nodeIndex1][nodeIndex2] = weight;
		adjMatrix[nodeIndex2][nodeIndex1] = weight;
		return true;
	}
	else if (weight < 0) {
		cout << "Weight value given was less than zero, must be nonnegative" << endl;
		return false;
	}
	else { //to account for an input larger than the number of vertices in the graph
		cout << "Node index out of range!!!" << nodeIndex1 << " " << nodeIndex2 << endl;
		return false;
	}
}

/*
 * Takes in a node index.  If the node is valid, then it scans the adjacency matrix and collects up
 * all the neighbors of the node, all nodes with nonnegative weights in the table.  It puts the node
 * indices into a vector and returns it.
 */
vector<int> WeightedMatrixGraph::getNeighbors(int nodeIndex) { //gets surrounding vertices of a given vertex
	vector<int> neighs;
	if (nodeIndex < numNodes) {
		for (int i = 0; i < numNodes; i++) {
			int wgt = adjMatrix[nodeIndex][i];
			if (wgt >= 0) {
				neighs.push_back(i);
			}
		}
	}
	else {
		cout << "OUT OF RANGE!!" << endl;
	}
	return neighs;
}


/*
 * Given two node indices, it returns the weight value between those nodes.  If the indices are not
 * valid then it returns -2.  NOTE: If not edge exists between the nodes, then it returns -1, which is
 * the value in the adjacency matrix that means "no edge"
 */
double WeightedMatrixGraph::getWeight(int nodeIndex1, int nodeIndex2) { //gets the weight of an edge between two given vertices
	if (nodeIndex1 < numNodes && nodeIndex2 < numNodes) {
		return adjMatrix[nodeIndex1][nodeIndex2];
	}
	else {
		cout << "Index OUT OF RANGE!! " << nodeIndex1 << " " << nodeIndex2 << endl;
		return -2;
	}
}


/*
 * An implementation of Prim's algorithm for finding a minimum spanning tree, it
 * builds the MST as a new WeightedMatrixGraph and returns it.
 * It has a minor hack for its input, an array of ints holding nodes to omit
 * from the MST, and the number of such ints.  It then checks if a node is
 * in that set, and if so it just skips it right along.  It only needs
 * numNodes - numInSet - 1 edges to be added.
 */
WeightedMatrixGraph* WeightedMatrixGraph::findMST(int sourceNode, int* skipSet, int numToSkip)
{
	//Looks through an array representing a graph and finds its minimal spanning tree.
	int visitedNodes[numNodes], numVisited;
	numTreeEdges=0;
	WeightedMatrixGraph* treeMatrix = new WeightedMatrixGraph(numNodes+1);
	numVisited = 1;
	visitedNodes[0] = sourceNode; //The source node

	int i;
	if (skipSet == NULL) {
		numToSkip = 0;
	}

	cout << endl << "*****The algorithm starts*****" << endl << endl;

	while ( numTreeEdges < (numNodes - numToSkip - 1) ) {
		// Find the least cost edge
		double minWeight=10000;
		int u = 1;
		int v = 1;
		for( i = 0; i < numVisited; i++) {
			int k = visitedNodes[i];
			for (int j = 0; j < numNodes; j++) {
				if (!isInSet(j, numToSkip, skipSet) ) {
					if ( adjMatrix[k][j] >= 0 && !isInSet(j, numVisited, visitedNodes) ) {
						//if the edge between nodes k and j has a defined weight and j has not already been visited
						cout << "---- checking edge from " << k << " to " << j << "  minWeight = " << minWeight << endl;
						if ( minWeight > adjMatrix[k][j] ) { //redefines minimumWeight if a given edge's weight is lower
							// than the current minimumWeight
							minWeight = adjMatrix[k][j];
							u = k; //start node position
							v = j; //destination node position
						}
					}
				}
			}
		}



		if (minWeight == 10000) { //Will only be accessed if no new edge can be located
			cout << "ERROR: Region not connected to anything else; ending program" << endl;
			cout << "Tree edges = " << numTreeEdges << " and num nodes = " << numNodes << endl;
			break;
		}
		else { //Otherwise, the edge with the lowest weight will be added.
			cout << "The edge included in the tree is :";
			cout << " < " << u << " , " << v << " > " << endl;
			treeMatrix->addEdge(u, v, minWeight);
			numTreeEdges++;

			//Alter the set partitions//
			visitedNodes[numVisited] = v;
			numVisited++;
		}
	}
	return treeMatrix;
}


/**
 * findRme: finds the regional minimum edges for a particular MST
 * a regional minimum edge is the lowest edge such that there is no node connected to it that contains a lower weight
 * This will be used in thew waterfall function
 */
int* WeightedMatrixGraph::findRme(){
	cout << "----------------Finding Regional Minimum----------------" << endl;
	bool marked[numNodes];	//keeps track of nodes already evaluated
	vector<Edge> potentialMinimumEdgeSets;	//stores the edges of potential regional minimum edges (RME)
	vector< vector<Edge> > minimumEdgeSet;	//stores the RMEs determined by the algorithm

	//set up a list of nodes that have been evaluated
	for (int node = 0; node < numNodes; node++) {
	  marked[node] = false;
	}
	//iterate through all of the vertices
	for (int node = 0; node < numNodes; node++) {
		//check if the node is marked
		if ( marked[node] == false ) {
			//empty the potential minimum edge set
			potentialMinimumEdgeSets.empty();
			int minValue = 500;  //minValue has to be larger than the maximum possible value in the WeightedMatrixGraph
			//set up potential nodes to search
			vector<int> nodesToSearch;
			nodesToSearch.push_back(node);
			//loop through nodesToSearch until there is nothing left
			while( nodesToSearch.size() > 0 ) {
				//pull out the first nodeToSearch
				int n1 = nodesToSearch[0];
				nodesToSearch.erase(nodesToSearch.begin());
				//get the neighbors
				vector<int> neighbors = getNeighbors(n1);
				//loop through the neighbors and consider whether or not they may be a minimum
				for (int i = 0; i < (int) neighbors.size(); i++) {
					int neigh = neighbors[i];
					double weight = getWeight(n1, neigh);
					Edge edge(n1, neigh, weight);
					//make sure that the neighbor isn't marked
					if (marked[neigh] == true) {
						if (isInSet(n1, potentialMinimumEdgeSets)) {//edge already in potential minimum edge set
							continue;
						}
						else if (weight > minValue) {//weight > minValue skip because it can't be an RME
							continue;
						}
						else if (weight < minValue) { //weight <= minvalue so clear potential and set minValue to its weight
							potentialMinimumEdgeSets.clear();
							nodesToSearch.clear();
							minValue = weight;
						}
					}
					else {  // neigh is not marked
						if (weight > minValue) {
							continue;
						}
						else if (weight < minValue) {	//weight is less than minimum value so make it the new potential minimum edge
							potentialMinimumEdgeSets.clear();
							cout<< edge.getNode1() << " " << edge.getNode2() << " " << edge.getWeight() << endl;
							potentialMinimumEdgeSets.push_back(edge);
							cout<< potentialMinimumEdgeSets.size() << endl;
							minValue = weight;
							nodesToSearch.clear();
							nodesToSearch.push_back(neigh);
						}
						else {  // weight == minValue
							if (potentialMinimumEdgeSets.empty()){	//if the potential sets are empty this is not actually an RME but a special case
								continue;
							}
							else{
								potentialMinimumEdgeSets.push_back(edge);//append n1->neigh to potentialMinimumEdgeSets;
								nodesToSearch.push_back(neigh);
							}
						}
					}
				}
				//mark the node as visited
				marked[n1] = true;
			}
			//these edges are RMEs so store them
			if (!potentialMinimumEdgeSets.empty()) {
				minimumEdgeSet.push_back(potentialMinimumEdgeSets);
				potentialMinimumEdgeSets.clear();
			}
		}
}
////printing out the minimum edge set
//for(int i = 0; i < (int) minimumEdgeSet.size(); i++){
//	cout << "set " << i << ":" << endl;
//	for(int q = 0; q < (int) minimumEdgeSet[i].size(); q++){
//		cout << minimumEdgeSet[i][q].getNode1() << " " << minimumEdgeSet[i][q].getNode2() << " " << minimumEdgeSet[i][q].getWeight() << endl;
//	}
//}

//return the merged regions
return mergeRegions(minimumEdgeSet);
}


/**
 * mergeRegions: helper method to findRme which handles the region merging
 */
int* WeightedMatrixGraph::mergeRegions(vector<vector<Edge> > minimumEdges){
	//set up a list of regions
	int* regions = new int[numNodes];
	for(int i = 0; i < numNodes; i++){
		regions[i] = i;
	}
	//go through the list of RMEs and merge the regions appropriately
	for(int i = 0; i < (int) minimumEdges.size(); i++){
		for(int q = 0; q < (int) minimumEdges[i].size(); q++){
			regions[minimumEdges[i][q].getNode2()] = minimumEdges[i][q].getNode1();
		}
	}
	return regions;
}


void WeightedMatrixGraph::printMatrix() {
	for (int row = 0; row < numNodes; row++) {
		for (int col = 0; col < numNodes; col++) {
			cout << setw(5) << adjMatrix[row][col];
		}
		cout << endl;
	}
}
/*
 * =========================================================================
 * Private methods below this point
 */


/*
 * Private method used to check whether a given node index is in the given set
 */
bool WeightedMatrixGraph::isInSet(int x, int numInSet, int* nodeSet) {
	//Determines whether a given node has been visited by the algorithm method.
	for(int i = 0; i < numInSet; i++)
		if(x == nodeSet[i])
			return true;
	return false;
}

bool WeightedMatrixGraph::isInSet(int x, vector<int> nodeSet) {
  //Determines whether a given node has been visited by the algorithm method.
  for(unsigned int i = 0; i < nodeSet.size(); i++) {
    if(x == nodeSet[i])
      return true;
  }
  return false;
}


/**
 * isInSet: checks if a vector of edges contains a given node
 */
bool WeightedMatrixGraph::isInSet(int x, vector<Edge>edgeSet) {
	for(unsigned int i = 0; i < edgeSet.size(); i++){
		if(edgeSet.at(i).getNode2() == x || edgeSet.at(i).getNode1() == x) {
			return true;
		}
	}
	return false;
}
