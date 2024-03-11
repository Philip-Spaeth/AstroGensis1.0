#include "Cluster.h"
#include "Particle.h"
#include <vector>
#include <random>
#include <iostream>

void Cluster::gasDarkMatterCluster(int startIndex, int endIndex, glm::dvec3 position, glm::dvec3 rotation, glm::dvec3 velocity, double maxRadius, double Masse, double stable, std::vector<Particle>& particles)
{
	halo.halo(startIndex, endIndex, position, rotation, velocity, maxRadius, Masse, stable, true, particles);
}
