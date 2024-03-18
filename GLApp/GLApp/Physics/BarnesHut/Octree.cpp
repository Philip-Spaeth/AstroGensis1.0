#include "Octree.h"
#include <iostream>
#include "Physics.h"
#include <thread>
#include <vector>
#include <functional>

Octree::Octree(glm::dvec3 center, double radius, double theta, int maxDepth, int particlesSize) 
{
	this->center = center;
	this->radius = radius;
	this->theta = theta;
	root = new Node(center, radius, theta, 0, maxDepth, false, glm::vec3(1,0,0), particlesSize);
}

Octree::~Octree() 
{
	delete root;
}
glm::dvec3 Octree::calculateForces(Physics* phy, Particle* particle, double softening, double a, double& potentialEngergy, double& calculations)
{
	return root->calcForce(phy,particle,root, softening,a, potentialEngergy, calculations);
}
void Octree::calcH(Particle* p)
{
	//calc h for all nodes not particles
	root->calcH(p);
}

void Octree::calcdensity(double h, double& medium, int& n)
{
	root->calcDensity(h, medium, n);
}

void Octree::buildTree(std::vector<Particle>* particles)
{
	int num_threads = std::thread::hardware_concurrency();
	std::vector<std::thread> threads;

	int n = particles->size(); // Gesamtanzahl der Iterationen
	int step = n / num_threads;
	for (int i = 0; i < num_threads; ++i) {
		//threads.emplace_back(insert, particles, i * step, (i + 1) * step);
		threads.emplace_back([this, &particles, i, step]() 
			{
			this->insert(particles, i * step, (i + 1) * step);
		});
	}

	for (auto& thread : threads)
	{
		thread.join();
	}
}

void Octree::insert(std::vector<Particle>* particles, int start, int end)
{
	for(int i = start; i < end; i++)
	{
		if ((*particles)[i].mass != 0)
		{
			Particle* p = &((*particles)[i]);
			root->insert(p);
		}
	}
}


double Octree::calculateTotalMassInSphere(Node* currentNode, glm::dvec3 targetPosition, double radius)
{
	double totalMass = 0.0;
	int maxDepth = 9;

	// Calculate the distance from the target position to the center of the current cube
	double distanceToCenter = glm::length(currentNode->center - targetPosition);

	if (currentNode->isLeaf)
	{
		// Cube is a leaf, check if it is inside the sphere
		if (distanceToCenter + currentNode->radius < radius && currentNode->index < maxDepth)
		{
			// Cube is inside the sphere, add its mass to the total mass
			return currentNode->mass;
		}
		else
		{
			// Cube is outside the sphere, no need to check further
			return 0.0;
		}
	}
	if (currentNode->index > maxDepth)
	{
		// Cube is not a leaf, check if it is far enough away from the target position
		if (distanceToCenter + currentNode->radius < radius * currentNode->theta)
		{
			// Cube is far enough away, add its mass to the total mass
			return currentNode->mass;
		}
	}
	else
	{
		// Cube intersects with the sphere, check its children
		for (Node* child : currentNode->child)
		{
			if (child != nullptr)
			{
				totalMass += calculateTotalMassInSphere(child, targetPosition, radius);
			}
		}
	}

	return totalMass;
}

double Octree::calculateTotalMassInSphere(glm::dvec3 targetPosition, double radius)
{
	return calculateTotalMassInSphere(root, targetPosition, radius);
}