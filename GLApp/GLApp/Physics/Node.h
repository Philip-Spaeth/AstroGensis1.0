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

	std::vector<Particle> particles;
	Node* children[8];

	void InsertToNode(Particle& particle);


	// zusammenfassen der Massen und Schwerpunkte
	Particle* centerOfMass;
	void calculateMassAndCenter();
	std::vector<Particle*> collectChildren(std::vector<Particle*> collectedParticles);

	void bringParticleToEnd();

	double accuracy = 0.1;

private:
	bool childrenCreated = false;
	glm::dvec3 globalCenter;
	double mass;
	Particle* centerOfMassChild;

	double accuracyIndex;
};

#endif // OCTREE_H