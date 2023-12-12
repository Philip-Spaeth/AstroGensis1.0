
#include "EllipticalGalaxy.h"

void EllipticalGalaxy::E0(int startIndex, int endIndex, glm::dvec3 position, glm::dvec3 rotation, glm::dvec3 velocity, double size, std::vector<Particle>& particles)
{

	int particleSize = endIndex + 1;

	double galaxyRadius = 1e21; // Radius der kugelförmigen Galaxie

	double starSpeed = 1;

	int i = 0;

	// Erstellen einer kugelförmigen Galaxie
	for (int j = startIndex; j != endIndex; j++)
	{
		// Mass Sagittarius A
		if (j == startIndex)
		{
			particles[j].position = position;
			particles[j].velocity = velocity;
			particles[j].mass = 1e36;
			particles[j].radius = 2;
			particles[j].color = glm::vec3(1, 1, 1);
		}
		else
		{
			double theta = physics.random(0, glm::pi<double>());
			double particles[j]. = physics.random(0, 2 * glm::pi<double>());
			double r = physics.random(0, physics.random(0, galaxyRadius));

			double x = r * std::sin(theta) * std::cos(phi);
			double y = r * std::sin(theta) * std::sin(phi);
			double z = 0;
			double distanceToCenter = glm::abs(glm::length(glm::dvec3(x, y, z)));
			if (distanceToCenter == 0)
			{
				distanceToCenter = 1;
			}
			particles[j].position = glm::dvec3(x, y, z) + position;
			particles[j].mass = 1e36;
			particles[j].radius = 0.1;
			//make the stars in the center brighter
			double brightness = 1 - (particles[j].CalculateDistance(particles[startIndex]) / galaxyRadius);
			particles[j].color = glm::vec3(brightness - 0.1, brightness - 0.1, brightness);
		}

		i++;
	}

	//calc the center of mass of the galaxy
	glm::dvec3 centerOfMass = glm::dvec3(0, 0, 0);
	double totalMassGalaxy = 0;
	for (int j = startIndex; j != endIndex; j++)
	{
		centerOfMass += particles[j].position * particles[j].mass;
		totalMassGalaxy += particles[j].mass;
	}
	centerOfMass /= totalMassGalaxy;

	// Calculate velocity
	for (int j = startIndex; j < endIndex; j++)
	{
		double distanceToCenter = glm::length(particles[j].position - centerOfMass);
		if (distanceToCenter != 0)
		{
			double v = 0;
			v = std::sqrt(physics.G * calcMassInRadius(startIndex, endIndex, position, rotation, particles, distanceToCenter) / distanceToCenter);

			// No need for critical section here
			particles[j].velocity = glm::dvec3(-v * std::sin(particles[j].angle), v * std::cos(particles[j].angle), 0) + velocity;
		}
	}
}

double SpiralGalaxy::calcMassInRadius(int startIndex, int endIndex, glm::dvec3 position, glm::dvec3 rotation, std::vector<Particle>& particles, double r)
{
	//calc mass
	double mass = 0;

	for (int i = startIndex; i < endIndex; i++)
	{
		double distanceToCenter = glm::length(particles[i].position - position);

		// Check if the particle is within the specified radius
		if (distanceToCenter <= r)
		{
			mass += particles[i].mass;
		}
	}


	//calc p for v = sqrt(G*(4/3*pi*r^2*p))
	//p = m / V
	double V = (static_cast<double>(3) / 4) * 3.14 * (r * r * r);
	double p = mass / V;

	//print out p
	//std::cout << p << std::endl;
	return mass;
}