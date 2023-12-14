#ifndef ELLIPTICALGALAXY_H
#define ELLIPTICALGALAXY_H

#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Particle.h"
#include "Physics.h"
#include <glm.hpp>

class EllipticalGalaxy
{
public:
	Physics physics;

	void E0(int startIndex, int endIndex, glm::dvec3 position, glm::dvec3 rotation, glm::dvec3 velocity, double size, std::vector<Particle>& particles);

	//helper funktions
	double EllipticalGalaxy::calcMassInRadius(int startIndex, int endIndex, glm::dvec3 position, glm::dvec3 rotation, std::vector<Particle>& particles, double r);

};

#endif