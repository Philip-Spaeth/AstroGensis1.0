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
	std::vector<glm::dvec3> velocities;
	
	Node* children[8];

	void InsertToNode(glm::dvec3 position, double mass, glm::dvec3 velocity);

	void buildTree();


	// zusammenfassen der Massen und Schwerpunkte
	Particle* centerOfMass;
	void summerizeMassAndCenter();
	std::vector<Particle*> collectChildren(std::vector<Particle*> collectedParticles);


	double accuracy = 0.001;

	// check if position is inside the node
	bool isInside(glm::dvec3 position);

	void calculateNewCenter();

private:
	bool childrenCreated = false;
	glm::dvec3 globalCenter;
	double mass;
	glm::dvec3 velocity;
	Particle* centerOfMassChild;

	double accuracyIndex;
	void createChildren();
	void sortParticlesIntoChildren();
};

#endif // OCTREE_H