#include "SpiralGalaxy.h"
#include <omp.h>
#include <iostream>

using namespace std;

void SpiralGalaxy::ellipticalOrbit(Particle& p, double m, double diskR, double r, double angle)
{
	double ellipticity = 0.1;
	double mainAxis = r;
	double minorAxis = r * (ellipticity + 1);

	double a = physics.random(0, 2 * 3.14);
	double x = mainAxis * std::cos(a);
	double y = minorAxis * std::sin(a);

	double z = 0;
	double depth = 0.2; // Die maximale Tiefe/Z-Höhe

	// Setzen Sie die Position
	p.position = glm::dvec3(x * std::cos(angle) - y * sin(angle), x * std::sin(angle) + y * cos(angle), z);


	double distanceToCenter = glm::abs(glm::length(p.position));

	if (distanceToCenter == 0)
	{
		distanceToCenter = 1;
	}
	double v = 0;
	double massInSphere = 0;
	massInSphere = m * distanceToCenter / diskR;
	v = std::sqrt(physics.G * massInSphere / distanceToCenter);

	//velocity tangent to the orbit 
	glm::dvec3 direction = glm::dvec3(-p.position.y, p.position.x, p.position.z);
	direction = glm::normalize(direction);
	p.velocity = direction * v;
}

void SpiralGalaxy::Sa(int startIndex, int endIndex, glm::dvec3 position, glm::dvec3 rotation, glm::dvec3 velocity, double size, std::vector<Particle>& particles)
{
	int particleSize = endIndex + 1 - startIndex;

	double galaxyRadius = 1e21 * size; // Radius der kugelförmigen Galaxie

	const double totalMass = 1e40; // Gesamtmasse der Galaxie
	double mass = totalMass / particleSize; // Masse eines einzelnen Partikels

	int i = 0;

	for (int j = startIndex; j != endIndex; j++)
	{
		double scaledI = i / (double)particleSize;
		double faktor = 1;
		double r = galaxyRadius * std::pow(scaledI, faktor) * (faktor * faktor);
		particles[j].angle = sqrt(r) * 2.0 * 3.14;

		//denisty wave angle with A and k
		double A = 2;
		double alpha = (i / (double)particleSize) * 2 * 3.14 * A;

		ellipticalOrbit(particles[j],totalMass, galaxyRadius, r, alpha);
		particles[j].position += position;
		particles[j].velocity += velocity;

		particles[j].mass = 1e36;
		particles[j].radius = 1;
		particles[j].darkMatter = false;
		particles[j].color = glm::vec3(1, 1, 1);
		i++;
	}
}

void SpiralGalaxy::Sb(int startIndex, int endIndex, glm::dvec3 position, glm::dvec3 rotation, glm::dvec3 velocity, double size, std::vector<Particle>& particles)
{
	int particleSize = endIndex + 1 - startIndex;

	double galaxyRadius = 1e21 * size; // Radius der kugelförmigen Galaxie

	double totalMass = 1e40; // Gesamtmasse der Galaxie
	double mass = totalMass / particleSize; // Masse eines einzelnen Partikels

	int i = 0;

	for (int j = startIndex; j != endIndex; j++)
	{
		double scaledI = i / (double)particleSize;
		double faktor = 1;
		double r = galaxyRadius * std::pow(scaledI, faktor) * (faktor * faktor);
		particles[j].angle = sqrt(r) * 2.0 * 3.14;

		//denisty wave angle with A and k
		double A = 1;
		double alpha = (i / (double)particleSize) * 2 * 3.14 * A;

		ellipticalOrbit(particles[j], totalMass, galaxyRadius, r, alpha);
		particles[j].position += position;
		particles[j].velocity += velocity;

		particles[j].mass = 1e36;
		particles[j].radius = 1;
		particles[j].darkMatter = false;
		particles[j].color = glm::vec3(1,1,1);
		i++;
	}
}

void SpiralGalaxy::Sc(int startIndex, int endIndex, glm::dvec3 position, glm::dvec3 rotation, glm::dvec3 velocity, double size, std::vector<Particle>& particles)
{
	int particleSize = endIndex + 1 - startIndex;

	double galaxyRadius = 1e21 * size; // Radius der kugelförmigen Galaxie

	double totalMass = 1e40; // Gesamtmasse der Galaxie
	double mass = totalMass / particleSize; // Masse eines einzelnen Partikels

	int i = 0;

	for (int j = startIndex; j != endIndex; j++)
	{
		double scaledI = i / (double)particleSize;
		double faktor = 1;
		double r = galaxyRadius * std::pow(scaledI, faktor) * (faktor * faktor);
		particles[j].angle = sqrt(r) * 2.0 * 3.14;

		//denisty wave angle with A and k
		double A = 0.5;
		double alpha = (i / (double)particleSize) * 2 * 3.14 * A;

		ellipticalOrbit(particles[j], totalMass, galaxyRadius, r, alpha);
		particles[j].position += position;
		particles[j].velocity += velocity;

		particles[j].mass = 1e36;
		particles[j].radius = 1;
		particles[j].darkMatter = false;
		particles[j].color = glm::vec3(1, 1, 1);
		i++;
	}
}