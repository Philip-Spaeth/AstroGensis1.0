#ifndef SYSTEMINIT_H
#define SYSTEMINIT_H

#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Particle.h"
#include "Physics.h"
#include <glm.hpp>
#include "Galaxies/BarredGalaxy.h"
#include "Galaxies/SpiralGalaxy.h"
#include "Galaxies/EllipticalGalaxy.h"

class SystemInit 
{
public:
	void start(std::vector<Particle>& particles);

private:
	Physics physics;

	BarredGalaxy barredGalaxy;
	SpiralGalaxy spiralGalaxy;
	EllipticalGalaxy ellipticalGalaxy;

	//solar system
	void solarSystem(std::vector<Particle>& particles);
	void ourSolarSystem(std::vector<Particle>& particles);
};

#endif