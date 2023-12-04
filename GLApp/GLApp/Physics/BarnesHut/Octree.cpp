#include "Octree.h"
#include <iostream>
#include "Physics.h"

Octree::Octree(glm::dvec3 center, double radius, double theta, int maxDepth) 
{
	this->center = center;
	this->radius = radius;
	this->theta = theta;
	root = new Node(center, radius, theta, 0, maxDepth);
}

Octree::~Octree() {
}

glm::dvec3 Octree::calculateForces(Particle& particle, double softening, double& potentialEngergy, double& calculations)
{
	if (particle.mass == 1e36)
	{
		return { 0,0,0 };
	}
	return root->calcForce(particle, softening, potentialEngergy, calculations);
}

void Octree::buildTree(std::vector<Particle>& particles) 
{
	for (int i = 0; i < particles.size(); i++) 
	{
		if (particles[i].mass != 0)
		{
			root->insert(particles[i]);
		}
	}
}

void Octree::clearTree() 
{
	root->clear();
}