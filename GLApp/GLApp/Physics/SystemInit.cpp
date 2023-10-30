#include "SystemInit.h"
#include <iostream>
#include <cmath>
#include <random>

void SystemInit::start(std::vector<std::vector<Particle>>& particles)
{
	solarSystem(particles);

	//ellipticalGalaxy(0, 999, { 0,0,0 }, { 0,0,0 }, particles);
	//ellipticalGalaxy(1000, 1999, { 1000, 200 ,0 }, { 0,0,0 }, particles);
	//spiralGalaxy(0, 999, { 0,0,0 }, { 0,0,0 }, particles);
	//spiralGalaxy(1000, 1999, { 2000, 500, 0 }, { - 0.1,0,0 }, particles);
}

void SystemInit::spiralGalaxy(int startIndex, int endIndex, glm::vec3 position, glm::vec3 velocity, std::vector<std::vector<Particle>>& particles)
{
	int size = endIndex + 1 - startIndex; 

	double diskRadius = 200;

	int numArms = 2;
	double armWidth = 0;
	double armDensity = 0.3;
	double interArmDensity = 1;
	double fixedRadius = 200;

	double depth = 10;

	int i = 0;

	// Erstellen einer Spiralgalaxie
	for (int j = startIndex; j != endIndex; j++)
	{
		// Schwarzes Loch in der Mitte
		if (j == startIndex)
		{
			Particle particle;
			particle.position = position;
			particle.velocity = velocity;
			particle.mass = 1e12;
			particle.radius = 2;
			particle.color = glm::vec3(1, 1, 1);
			particles[0][startIndex] =  particle;
		}

		// Sterne in Spiralarmen um das schwarze Loch
		else
		{
			Particle particle;

			double r = physics.random(0,diskRadius);

			r = r * std::exp(1 * r / diskRadius);

			//Fz = Fg
			//m*v^2/r = G * M * m / r^2
			// v^2 = G * M / r
			//v = sqrt(G * M / r)
			double v = std::sqrt((physics.G * particles[0][startIndex].mass) / r);

			double armAngle = 2 * 3.14 * (j - startIndex) / numArms; // Winkel für die Anzahl der Arme

			particle.position = glm::vec3(r * std::cos(armAngle), r * std::sin(armAngle), physics.random(-depth, depth)) + position;
			particle.velocity = glm::vec3(-v * std::sin(armAngle), v * std::cos(armAngle), 0) + velocity;
			particle.mass = 0.5;
			particle.radius = 0.01;
			//particle.color = glm::vec3(1, 1, 1);
			particles[0][j] = particle;
		}
		i++;
	} 
}	

void SystemInit::ellipticalGalaxy(int startIndex, int endIndex, glm::vec3 position, glm::vec3 velocity, std::vector<std::vector<Particle>>& particles)
{
	int size = endIndex + 1;

	double diskRadius = 200;

	double starSpeed = 1;

	double depth = 20;

	int i = 0;
	// Erstellen einer Spiralgalaxie
	for (int j = startIndex; j < size; j++)
	{
		// Schwarzes Loch in der Mitte
		if (j == startIndex)
		{
			particles[0][j].position = position;
			particles[0][j].velocity = velocity;
			particles[0][j].mass = 1e11;
			particles[0][j].radius = 2;
			particles[0][j].color = glm::vec3(1, 1, 1);
		}

		else
		{
			Particle particle;
			double r = (diskRadius / size) * i;
			double angle = physics.random(0, 2 * 3.14);

			//Fz = Fg
			//m*v^2/r = G * M * m / r^2
			// v^2 = G * M / r
			//v = sqrt(G * M / r)
			double v = std::sqrt((physics.G * particles[0][startIndex].mass) / r) * starSpeed;

			particles[0][j].position = glm::vec3(r * std::cos(angle), r * std::sin(angle), physics.random(-depth , depth)) + position;
			particles[0][j].velocity = glm::vec3(-v * std::sin(angle), v * std::cos(angle), 0) + velocity;
			particles[0][j].mass = 1;
			particles[0][j].radius = 0.01;
			particle.color = glm::vec3(1, 1, 1);
		}
		i++;
	}
	//set the other particles to 0
	for (int j = endIndex; j < particles.size(); j++)
	{
		Particle particle;
		particle.position = glm::vec3(1e20, 0, 0);
		particle.velocity = glm::vec3(0, 0, 0);
		particle.mass = 0;
		particle.radius = 0;
		particle.color = glm::vec3(0, 0, 0);
		particles[0][j] = particle;
	}
}


void SystemInit::solarSystem(std::vector<std::vector<Particle>>& particles)
{
	Physics physics;

	double distanceFaktor = 300;

	for (int j = 0; j < physics.particlesSize; j++)
	{
		//One sun with a lot of mass
		if (j == 0)
		{
			Particle particle;
			particle.position = glm::vec3(0, 0, 0);
			particle.velocity = glm::vec3(0, 0, 0);
			particle.mass = 1000000;
			particle.radius = 6;
			particle.color = glm::vec3(1, 1, 0);
			particles[0][j] = particle;
		}
		else
		{
			Particle particle;
			particle.position = glm::vec3(j * distanceFaktor, 0,0);

			//Fz = Fg
			//m*v^2/r = G * M * m / r^2
			// v^2 = G * M / r
			//v = sqrt(G * M / r)

			double v = std::sqrt((physics.G * particles[0][0].mass) / (j * distanceFaktor));

			particle.velocity = glm::vec3(0, v, 0);
			particle.mass = 100;

			//diffrent color for every planet

			int color = j % 3;
			if (color == 0)
			{
				particle.color = glm::vec3(1, 0, 0);
			}
			else if (color == 1)
			{
				particle.color = glm::vec3(0, 1, 0);
			}
			else if (color == 2)
			{
				particle.color = glm::vec3(0, 0, 1);
			}

			//diffrent size for every planet
			int size = j % 3;
			if (size == 0)
			{
				particle.radius = 2;
			}
			else if (size == 1)
			{
				particle.radius = 3;
			}
			else if (size == 2)
			{
				particle.radius = 4;
			}
			particles[0][j] = particle;
		}
	}
}

