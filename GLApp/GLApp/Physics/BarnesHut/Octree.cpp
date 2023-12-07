#include "Octree.h"
#include <iostream>
#include "Physics.h"
#include <thread>
#include <vector>
#include <functional>

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
	if (particle.mass == 1e41)
	{
		return { 0,0,0 };
	}
	return root->calcForce(particle, softening, potentialEngergy, calculations);
}

void Octree::buildTree(std::vector<Particle>& particles) 
{
	int num_threads = std::thread::hardware_concurrency();
	std::vector<std::thread> threads;

	int n = particles.size(); // Gesamtanzahl der Iterationen
	int step = n / num_threads;
	for (int i = 0; i < num_threads; ++i) {
		//threads.emplace_back(insert, particles, i * step, (i + 1) * step);
		threads.emplace_back([this, &particles, i, step]() {
			this->insert(particles, i * step, (i + 1) * step);
		});
	}

	for (auto& thread : threads)
	{
		thread.join();
	}
}

void Octree::insert(std::vector<Particle>& particles, int start, int end)
{
	for(int i = start; i < end; i++)
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