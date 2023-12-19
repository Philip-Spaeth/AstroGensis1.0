
#include "EllipticalGalaxy.h"

void EllipticalGalaxy::E0(int startIndex, int endIndex, glm::dvec3 position, glm::dvec3 rotation, glm::dvec3 velocity, double size, std::vector<Particle>& particles)
{

	int particleSize = endIndex + 1;

	double galaxyRadius = 1e21; // Radius der kugelförmigen Galaxie

	double totalMass = 1e40; // Gesamtmasse der Galaxie
	double mass = totalMass / particleSize; // Masse eines einzelnen Partikels

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
			particles[j].mass = totalMass / particleSize;
			particles[j].radius = 1;
			particles[j].color = glm::vec3(1, 1, 1);
		}
		else
		{
			double theta = physics.random(0, glm::pi<double>());
			particles[j].angle = physics.random(0, 2 * glm::pi<double>());
			//linier distribution
			double scaledI = i / (double)particleSize;
			double r = galaxyRadius * std::pow(scaledI, 1.0 / 3.0);

			double x = r * std::sin(theta) * std::cos(particles[j].angle);
			double y = r * std::sin(theta) * std::sin(particles[j].angle);
			double z = 0;
			double distanceToCenter = glm::abs(glm::length(glm::dvec3(x, y, z)));
			if (distanceToCenter == 0)
			{
				distanceToCenter = 1;
			}
			particles[j].position = glm::dvec3(x, y, z) + position;

			double v = 0;
			double massInSphere = 0;
			massInSphere = totalMass * distanceToCenter / galaxyRadius;
			//double mass = octree.calculateTotalMassInSphere(position, distanceToCenterb);
			v = std::sqrt(physics.G * massInSphere / distanceToCenter);
			// No need for critical section here
			particles[j].velocity = glm::dvec3(-v * std::sin(particles[j].angle), v * std::cos(particles[j].angle), 0) + velocity;

			particles[j].mass = totalMass / particleSize;
			particles[j].radius = 1;
			//make the stars in the center brighter
			//double brightness = 1 - (particles[j].CalculateDistance(particles[startIndex]) / galaxyRadius);
			//particles[j].color = glm::vec3(brightness - 0.1, brightness - 0.1, brightness);
		}

		i++;
	}
}