
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

	double bulgeScale = 0.07;
	double diskScale = 0.03;
	double bulgeR = diskR / 30;

	double z = 0;
	// Berechnen der Z-Koordinate basierend auf der Position im Bulge oder in der Disk
	if (r < bulgeR)
	{
		z = physics.gaussianRandom() * bulgeScale * diskR;
	}
	else 
	{
		z = physics.gaussianRandom() * diskScale * diskR;
	}

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

	glm::mat4 transform = glm::mat4(1.0f); // Identity matrix
	// Apply rotations
	transform = glm::rotate(transform, (float)rotation.x, glm::vec3(1, 0, 0));
	transform = glm::rotate(transform, (float)rotation.y, glm::vec3(0, 1, 0));
	transform = glm::rotate(transform, (float)rotation.z, glm::vec3(0, 0, 1));


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

		glm::vec4 pos = glm::vec4(particles[j].position - position, 1.0);
		glm::vec4 vel = glm::vec4(particles[j].velocity, 1.0);

		pos = transform * pos;
		vel = transform * vel;

		particles[j].position = glm::dvec3(pos) + position;
		particles[j].velocity = glm::dvec3(vel);
	}
}

void SpiralGalaxy::Sb(int startIndex, int endIndex, glm::dvec3 position, glm::dvec3 rotation, glm::dvec3 velocity, double size, std::vector<Particle>& particles)
{
	int particleSize = endIndex + 1 - startIndex;

	double galaxyRadius = 1e21 * size; // Radius der kugelförmigen Galaxie

	double totalMass = 1e40; // Gesamtmasse der Galaxie
	double mass = totalMass / particleSize; // Masse eines einzelnen Partikels

	int i = 0;

	glm::mat4 transform = glm::mat4(1.0f); // Identity matrix

	// Apply rotations
	transform = glm::rotate(transform, (float)rotation.x, glm::vec3(1, 0, 0));
	transform = glm::rotate(transform, (float)rotation.y, glm::vec3(0, 1, 0));
	transform = glm::rotate(transform, (float)rotation.z, glm::vec3(0, 0, 1));


	for (int j = startIndex; j != endIndex; j++)
	{
		double scaledI = i / (double)particleSize;
		double faktor = 1;
		double r = galaxyRadius * std::pow(scaledI, faktor) * (faktor * faktor);
		particles[j].angle = sqrt(r) * 2.0 * 3.14;

		//denisty wave angle with A and k
		double A = 0.7;
		double alpha = (i / (double)particleSize) * 2 * 3.14 * A;

		ellipticalOrbit(particles[j], totalMass, galaxyRadius, r, alpha);
		particles[j].position += position;
		particles[j].velocity += velocity;

		particles[j].mass = 1e36;
		particles[j].radius = 1;
		particles[j].darkMatter = false;
		particles[j].color = glm::vec3(1,1,1);
		i++;

		glm::vec4 pos = glm::vec4(particles[j].position - position, 1.0);
		glm::vec4 vel = glm::vec4(particles[j].velocity, 1.0);

		pos = transform * pos;
		vel = transform * vel;

		particles[j].position = glm::dvec3(pos) + position;
		particles[j].velocity = glm::dvec3(vel);
	}
}

void SpiralGalaxy::Sc(int startIndex, int endIndex, glm::dvec3 position, glm::dvec3 rotation, glm::dvec3 velocity, double size, std::vector<Particle>& particles)
{
	int particleSize = endIndex + 1 - startIndex;

	double galaxyRadius = 1e21 * size; // Radius der kugelförmigen Galaxie

	double totalMass = 1e40; // Gesamtmasse der Galaxie
	double mass = totalMass / particleSize; // Masse eines einzelnen Partikels

	int i = 0;

	glm::mat4 transform = glm::mat4(1.0f); // Identity matrix

	// Apply rotations
	transform = glm::rotate(transform, (float)rotation.x, glm::vec3(1, 0, 0));
	transform = glm::rotate(transform, (float)rotation.y, glm::vec3(0, 1, 0));
	transform = glm::rotate(transform, (float)rotation.z, glm::vec3(0, 0, 1));


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

		glm::vec4 pos = glm::vec4(particles[j].position - position, 1.0);
		glm::vec4 vel = glm::vec4(particles[j].velocity, 1.0);

		pos = transform * pos;
		vel = transform * vel;

		particles[j].position = glm::dvec3(pos) + position;
		particles[j].velocity = glm::dvec3(vel);
	}
}