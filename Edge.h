/*
 * Edge.h
 *
 *  Created on: Jul 20, 2011
 *      Author: macalester
 */

#ifndef EDGE_H_
#define EDGE_H_

class Edge{

protected:
	int node1;
	int node2;
	double weight;

public:
	Edge(int n1, int n2, double weight);
	int getNode1();
	int getNode2();
	double getWeight();


};
#endif /* EDGE_H_ */
