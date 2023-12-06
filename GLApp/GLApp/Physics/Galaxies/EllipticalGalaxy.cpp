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
			double phi = physics.random(0, 2 * glm::pi<double>());
			double r = physics.random(0, physics.random(0, galaxyRadius));

			double x = r * std::sin(theta) * std::cos(phi);
			double y = r * std::sin(theta) * std::sin(phi);
			double z = r * std::cos(theta);

			double v = std::sqrt((physics.G * particles[startIndex].mass) / r) * starSpeed;

			particles[j].position = glm::dvec3(x, y, z) + position;
			particles[j].velocity = glm::dvec3(-v * std::sin(phi), v * std::cos(phi), 0) + velocity;
			particles[j].mass = 1e30;
			particles[j].radius = physics.random(0.1, 2);
			//make the stars in the center brighter
			double brightness = 1 - (particles[j].CalculateDistance(particles[startIndex]) / galaxyRadius);
			particles[j].color = glm::vec3(brightness - 0.1, brightness - 0.1, brightness);
		}

		i++;
	}
}