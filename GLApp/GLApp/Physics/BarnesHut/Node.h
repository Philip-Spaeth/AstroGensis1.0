#ifndef NODE_H
#define NODE_H

#include <vector>
#include <glm.hpp>
#include "Particle.h"

class Particle;

class Node 
{
public:
	Node(glm::dvec3 center, double radius, double theta, int index);
	~Node();

	void insert(Particle& p);
	glm::dvec3 calcForce(Particle& p);
	int GetHeight() const;
    void calcMass();

	Node* child[8] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };

	int index = 0;
	bool isLeaf = false;
	glm::dvec3 center;
	glm::dvec3 massCenter;
	double mass = 0;
	double radius = 0;
	double theta = 0;

	Particle particle;
};

#endif