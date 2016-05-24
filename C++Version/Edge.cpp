/*
 * Edge.cpp
 *
 *  Created on: Jul 20, 2011
 *      Author: macalester
 */
#include "Edge.h"

Edge::Edge(int n1, int n2, double weight){
	this -> node1 = n1;
	this -> node2 = n2;
	this -> weight = weight;
}

int Edge::getNode1(){
	return node1;
}

int Edge::getNode2(){
	return node2;
}

double Edge::getWeight(){
	return weight;
}
