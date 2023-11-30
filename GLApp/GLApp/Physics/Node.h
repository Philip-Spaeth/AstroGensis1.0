#ifndef NODE_H
#define NODE_H

#include <vector>
#include <glm.hpp>
#include "Particle.h"


class Node 
{
public:
	Node(glm::dvec3 center, double halfsize);
	~Node();

	double halfSize;

	double mass;
	glm::dvec3 center;

	std::vector<Particle> particles;
	Node* children[8];

	void InsertToNode(Particle& particle);


	// Inside Node class
	glm::dvec3 calculateGravitationalForceBarnesHut(Particle& particle, double G, double softening, double deltaTime, double theta);

private:
	bool childrenCreated = false;
};

#endif // OCTREE_H