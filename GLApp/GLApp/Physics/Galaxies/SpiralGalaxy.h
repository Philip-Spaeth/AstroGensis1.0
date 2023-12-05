#ifndef SPIRALGALAXY_H
#define SPIRALGALAXY_H

#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Particle.h"
#include "Physics.h"
#include <glm.hpp>

class SpiralGalaxy
{
public:
	Physics physics;

	void Sb(int startIndex, int endIndex, glm::dvec3 position, glm::dvec3 rotation, glm::dvec3 velocity, double size, std::vector<Particle>& particles);
};

#endif