#include "SpiralGalaxy.h"
#include <omp.h>
#include <iostream>

using namespace std;

void SpiralGalaxy::ellipticalOrbit(Particle& p, double r, double angle)
{
	double ellipticity = 0.1;
	double mainAxis = r;
	double minorAxis = r * (ellipticity + 1);

	double a = physics.random(0, 2 * 3.14);
	double x = mainAxis * std::cos(a);
	double y = minorAxis * std::sin(a);

	p.position = glm::dvec3(x * std::cos(angle) - y * sin(angle), x * std::sin(angle) + y * cos(angle), 0);

	double distanceToCenter = glm::abs(glm::length(p.position));
	if (distanceToCenter == 0)
	{
		distanceToCenter = 1;
	}

	double v = 0;
	v = std::sqrt(physics.G * 1e40 / distanceToCenter);

	//velocity tangent to the orbit 
	glm::dvec3 direction = glm::dvec3(-p.position.y, p.position.x, 0);
	direction = glm::normalize(direction);
	p.velocity = direction * v;
}

void SpiralGalaxy::densityWaveSb(int startIndex, int endIndex, glm::dvec3 position, glm::dvec3 rotation, glm::dvec3 velocity, double size, std::vector<Particle>& particles)
{
	int particleSize = endIndex + 1;

	double galaxyRadius = 1e21; // Radius der kugelförmigen Galaxie

	double totalMass = 1e40; // Gesamtmasse der Galaxie
	double mass = totalMass / particleSize; // Masse eines einzelnen Partikels

	int i = 0;

	// Erstellen einer kugelförmigen Galaxie
	for (int j = startIndex; j != endIndex; j++)
	{
		// Mass Sagittarius A
		if (j == startIndex)
		{
			particles[j].position = position;
			particles[j].velocity = velocity;
			particles[j].mass = 1e40;
			particles[j].radius = 1;
			particles[j].color = glm::vec3(1, 1, 1);
		}
		else
		{
			double r = galaxyRadius * std::pow(i / (double)particleSize, 1.0);
			particles[j].angle = sqrt(r) * 2.0 * 3.14;

			//denisty wave angle with A and k
			double A = 1;
			double alpha = (i / (double)particleSize) * 2 * 3.14 * A;

			ellipticalOrbit(particles[j], r, alpha);

			particles[j].mass = 1e36;
			particles[j].radius = 1;
			particles[j].darkMatter = false;
			particles[j].color = glm::vec3(1,1,1);
		}
		i++;
	}
}


void SpiralGalaxy::Sb(int startIndex, int endIndex, glm::dvec3 position, glm::dvec3 rotation, glm::dvec3 velocity, double size, std::vector<Particle>& particles)
{
	std::cout << std::endl;
	std::cout << "Calculating Spiral Galaxy start Positions and Velocity ..." << std::endl;

	int particleSize = endIndex + 1 - startIndex;

	double BulgeRadius = 4.019e20;
	double diskRadius = 8.02e20;
	double haloRadius = 1.89e21;

	int numArms = 2;
	int i = 0;

	//verteillung der masse 
	double totalMass = 2.5641e42;

	double percentBulge = 0.05;
	double bulgeMass = percentBulge * totalMass;
	double bulgeMassPerParticle = bulgeMass / (particleSize * percentBulge);

	double percentDisk = 0.05;
	double diskMass = percentDisk * totalMass;
	double diskMassPerParticle = diskMass / (particleSize * percentDisk);

	double percentDarkMatter = 0.90;
	double darkMatterMass = percentDarkMatter * totalMass;
	double darkMatterMassPerParticle = darkMatterMass / (particleSize * percentDarkMatter);


	//split the size of the galaxy in 3 parts
	int bulgeSize = particleSize * percentBulge;
	int diskSize = particleSize * percentDisk;
	int darkMatterSize = particleSize * percentDarkMatter;

	int bulgeStart = startIndex;
	int bulgeEnd = bulgeStart + bulgeSize;

	int diskStart = bulgeEnd;
	int diskEnd = diskStart + diskSize;

	int darkMatterStart = diskEnd;
	int darkMatterEnd = darkMatterStart + darkMatterSize;

	// Erstellen einer Spiralgalaxie
	for (int j = startIndex; j != endIndex; j++)
	{
		//Dense bulge in the center
		if (j >= bulgeStart && j < bulgeEnd)
		{
			// Schwarzes Loch in der Mitte
			if (j == startIndex)
			{
				particles[j].position = position;
				particles[j].velocity = velocity;
				particles[j].mass = bulgeMassPerParticle;
				particles[j].radius = 1;
				particles[j].color = glm::vec3(1, 1, 1);
			}
			else
			{
				//random radius and depth
				//double r = physics.random(0, physics.random(0, BulgeRadius));
				//double depth = physics.random(0, physics.random(0, physics.random(0, BulgeRadius)));
				//particles[j].angle = physics.random(0, 2 * 3.14);
				//not random radius and depth
				double r = physics.random(0, physics.random(0, BulgeRadius));
				double depth = physics.random(0, physics.random(0, physics.random(0, BulgeRadius)));
				particles[j].angle = j * (-2 * 3.14) / bulgeSize;
				particles[j].angle = physics.random(0, 2 * 3.14);
				particles[j].position = glm::dvec3(r * std::cos(particles[j].angle), r * std::sin(particles[j].angle), physics.random(-depth, depth)) + position;
				particles[j].mass = bulgeMassPerParticle;
				particles[j].radius = physics.random(0.1, 2);
				particles[j].color = glm::vec3(1, 1, 1);
			}
		}
		//disk 
		else if (j >= diskStart && j < diskEnd)
		{
			double r = physics.random(0, physics.random(0, diskRadius));
			double depth = physics.random(0, physics.random(0, physics.random(0, BulgeRadius))) / 3;
			particles[j].angle = 2 * 3.14 * ((j - startIndex)) / numArms;
			particles[j].position = glm::dvec3(r * std::cos(particles[j].angle), r * std::sin(particles[j].angle), physics.random(-depth,depth)) + position;
			particles[j].mass = diskMassPerParticle;
			particles[j].radius = physics.random(0.1, 2);
			particles[j].color = glm::vec3(1, 1, 1);
		}

		//dark matter in the outer regions
		else if (j >= darkMatterStart && j < darkMatterEnd)
		{
			double r = physics.random(physics.random(0, physics.random(0, physics.random(0, physics.random(0, physics.random(0, physics.random(0, haloRadius)))))), haloRadius);
			double depth = physics.random(0, physics.random(0, physics.random(0, BulgeRadius))) / 3;

			particles[j].angle = physics.random(0, 2 * 3.14);
			particles[j].position = glm::dvec3(r * std::cos(particles[j].angle), r * std::sin(particles[j].angle), physics.random(-depth, depth)) + position;
			particles[j].mass = darkMatterMassPerParticle;
			particles[j].radius = physics.random(0.1, 2);
			particles[j].color = glm::vec3(1, 1, 1);
			particles[j].darkMatter = true;
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
			v = std::sqrt(physics.G * calcMassInRadius(startIndex,endIndex,position,rotation,particles,distanceToCenter) / distanceToCenter);

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