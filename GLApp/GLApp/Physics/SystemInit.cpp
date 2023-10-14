#include "SystemInit.h"
#include <iostream>
#include <cmath>


void SystemInit::start(std::vector<std::vector<Particle>>& particles)
{

	//solarSystem(particles);

	spiralGalaxy(0, { 0,0,0 }, { 0,0,0 }, particles);
	//galaxy(1000, { 5000000, 3000,0 }, { 0,0,0 }, particles);
}

void SystemInit::spiralGalaxy(int size, glm::vec3 position, glm::vec3 velocity, std::vector<std::vector<Particle>>& particles)
{
	// Ändern: Setzen von Werten für diskRadius und starSpeed
	double diskRadius = 200;
	double starSpeed = 0.03;

	int numArms = 4;
	double armWidth = 0;
	double armDensity = 0.3;
	double interArmDensity = 1;

	double depth = 10;

	// Erstellen einer Spiralgalaxie
	for (int j = size; j < physics.particlesSize; j++)
	{
		// Schwarzes Loch in der Mitte
		if (j == size)
		{
			particles[0][j].position = position;
			particles[0][j].velocity = velocity;
			particles[0][j].mass = 1e12;
			particles[0][j].radius = 2;
			particles[0][j].color = glm::vec3(1, 1, 0);
		}

		// Sterne in Spiralarmen um das schwarze Loch
		else
		{
			Particle particle;
			double r = physics.random(0.0, diskRadius);
			double angle = physics.random(0, 2 * armWidth);

			// Ändern: Verwende eine logarithmische Verteilung der Radien, um die Spiralarme zu erzeugen
			r = r * std::exp(starSpeed * r / diskRadius);
			double v = std::sqrt((physics.G * particles[0][size].mass) / r) * starSpeed;
			double armAngle = 2 * 3.14 * (j - size) / numArms; // Winkel für die Anzahl der Arme

			double density = (angle < armDensity) ? 1.0 : interArmDensity;

			particle.position = glm::vec3(r * std::cos(angle + armAngle), r * std::sin(angle + armAngle), physics.random(-depth, depth)) + particles[0][size].position + position;
			particle.velocity = glm::vec3(-v * std::sin(angle + armAngle), v * std::cos(angle + armAngle), 0) + particles[0][size].velocity + velocity;
			particle.mass = 0.5;
			particle.radius = 0.5;
			particle.color = glm::vec3(1, 1, 1);
			particles[0][j] = particle;
		}
	}
}	


void SystemInit::solarSystem(std::vector<std::vector<Particle>>& particles)
{

	//works the best with 10 particles
	Physics physics;

	double distanceFaktor = 30;
	double velocityFaktor = 0.05;
	double antivelocityFaktor = 0.05;

	for (int j = 0; j < physics.particlesSize; j++)
	{
		//One sun with a lot of mass
		if (j == 0)
		{
			Particle particle;
			particle.position = glm::vec3(0, 0, 0);
			particle.velocity = glm::vec3(0, 0, 0);
			particle.mass = 1e14;
			particle.radius = 4;
			particle.color = glm::vec3(1, 1, 0);
			particles[0][j] = particle;
		}
		else
		{
			Particle particle;
			particle.position = glm::vec3(j * distanceFaktor, 0,0);
			double v;
			if (j > 1)
			{
				v = std::sqrt((physics.G * particles[0][0].mass) / (j)) * velocityFaktor;
			}
			else
			{
				v = std::sqrt((physics.G * particles[0][0].mass) / (j)) * antivelocityFaktor;
			}

			particle.velocity = glm::vec3(0, v, 0);
			particle.mass = 1;
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
				particle.radius = 0.5;
			}
			else if (size == 1)
			{
				particle.radius = 1;
			}
			else if (size == 2)
			{
				particle.radius = 2;
			}
			particles[0][j] = particle;
		}
	}
}