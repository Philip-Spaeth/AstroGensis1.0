#include "SystemInit.h"
#include <iostream>
#include <cmath>


void SystemInit::start(std::vector<std::vector<Particle>>& particles)
{
	galaxy(0, { 0,0,0 }, { 0,0,0 }, particles);
	//galaxy(1000, { 5000000, 3000,0 }, { 0,0,0 }, particles);
}

void SystemInit::galaxy(int size, glm::vec3 position, glm::vec3 velocity, std::vector<std::vector<Particle>>& particles)
{
	// Change: Set values for diskRadius and starSpeed to good defaults
	double diskRadius = 200;
	double starSpeed = 0.03;

	//Creating a galaxy like the milky way
	for (int j = size; j < physics.particlesSize; j++)
	{
		//black hole in the middle
		if (j == size)
		{
			particles[0][j].position = position;
			particles[0][j].velocity = velocity;
			particles[0][j].mass = 1e12;
			particles[0][j].radius = 2;
			particles[0][j].color = glm::vec3(1, 1, 0);
		}

		//stars on a disk around the black hole with velocity so the stars stay on the orbit arod the vlack hole
		else
		{
			Particle particle;
			double r = physics.random(0.0, diskRadius);
			double angle = physics.random(0, 2 * 3.14);
			double v = std::sqrt((physics.G * particles[0][size].mass) / r) * starSpeed;
			particle.position = glm::vec3(r * std::cos(angle), r * std::sin(angle), 0) + particles[0][size].position + position;
			particle.velocity = glm::vec3(-v * std::sin(angle), v * std::cos(angle), 0) + particles[0][size].velocity + velocity;
			particle.mass = 0.5;
			particle.radius = 0.5;
			particle.color = glm::vec3(1, 1, 1);
			particles[0][j] = particle;
		}


	}
}

// not working
void SystemInit::solarSystem(std::vector<std::vector<Particle>>& particles)
{
	Physics physics;

	for (int j = 0; j < physics.particlesSize; j++)
	{
		//One sun with a lot of mass
		if (j == 0)
		{
			Particle particle;
			particle.position = glm::vec3(0, 0, 0);
			particle.velocity = glm::vec3(0, 0, 0);
			particle.mass = 1e14;
			particle.radius = 2;
			particle.color = glm::vec3(1, 1, 0);
			particles[0][j] = particle;
		}
		else
		{
			Particle particle;
			particle.position = glm::vec3(physics.random(-100, 100), physics.random(-100, 100), physics.random(-100, 100));
			particle.velocity = glm::vec3(physics.random(-1, 1), physics.random(-1, 1), physics.random(-1, 1));
			particle.mass = 1;
			particle.color = glm::vec3(1, 1, 1);
			particles[0][j] = particle;
		}
	}
}