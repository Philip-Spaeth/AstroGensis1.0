#ifndef CLUSTER_H
#define CLUSTER_H
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Particle.h"
#include "Physics.h"
#include "Halo.h"
#include <glm.hpp>

class Cluster
{
public:
	Cluster() {};
	Physics* physics;
	Halo halo;
	void gasDarkMatterCluster(int startIndex, int endIndex, glm::dvec3 position, glm::dvec3 rotation, glm::dvec3 velocity, double maxRadius, double Masse, double stable, std::vector<Particle>& particles);
};
#endif
