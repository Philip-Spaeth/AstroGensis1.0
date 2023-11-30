#ifndef NODE_H
#define NODE_H

#include <vector>
#include <glm.hpp>
#include "Particle.h"


class Node 
{
public:
	Node(glm::dvec3 center, double halfsize, double accuracyIndex);
	~Node();

	double halfSize;

	glm::dvec3 center;

	std::vector<glm::dvec3> positions;
	std::vector<double> masses;

	
	Node* children[8];

	void InsertToNode(glm::dvec3 position, double mass);

	void buildTree();


	// zusammenfassen der Massen und Schwerpunkte
	Particle* centerOfMass;
	void calculateMassAndCenter();
	std::vector<Particle*> collectChildren(std::vector<Particle*> collectedParticles);


	double accuracy = 0.1;

	// check if position is inside the node
	bool isInside(glm::dvec3 position);

private:
	bool childrenCreated = false;
	glm::dvec3 globalCenter;
	double mass;
	Particle* centerOfMassChild;

	double accuracyIndex;
	void createChildren();
	void sortParticlesIntoChildren();
};

#endif // OCTREE_H