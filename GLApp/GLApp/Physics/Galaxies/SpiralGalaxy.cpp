#include "SpiralGalaxy.h"

void SpiralGalaxy::Sb(int startIndex, int endIndex, glm::dvec3 position, glm::dvec3 rotation, glm::dvec3 velocity, double size, std::vector<Particle>& particles)
{
	int particleSize = endIndex + 1 - startIndex;

	double diskRadius = 1e21;
	double BulgeRadius = 3e20;

	int numArms = 2;
	int i = 0;

	//verteillung der masse 
	double totalMass = 1e42;
	double blackHoleMass = 1e36;

	double percentBulge = 0.5;
	double bulgeMass = percentBulge * totalMass;

	double percentDisk = 0.3;
	double diskMass = percentDisk * totalMass;

	double percentDarkMatter = 0.2;
	double darkMatterMass = percentDarkMatter * totalMass;
	double haloRadius = 1e22;


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
				particles[j].mass = blackHoleMass;
				particles[j].radius = 2;
				particles[j].color = glm::vec3(1, 1, 1);
			}
			else
			{
				double r = physics.random(0, physics.random(0, BulgeRadius));
				double depth = physics.random(0, physics.random(0, physics.random(0, BulgeRadius)));

				double angle = physics.random(0, 2 * 3.14);

				particles[j].position = glm::dvec3(r * std::cos(angle), r * std::sin(angle), physics.random(-depth, depth)) + position;
				double distanceToCenter = glm::length(particles[j].position - position);
				double v = std::sqrt((physics.G * blackHoleMass) / distanceToCenter);
				particles[j].velocity = glm::dvec3(-v * std::sin(angle), v * std::cos(angle), 0) + velocity;
				particles[j].mass = 1e30;
				particles[j].radius = physics.random(0.1, 2);
				particles[j].color = glm::vec3(1, 1, 1);
			}
		}
		//disk 
		else if (j >= diskStart && j < diskEnd)
		{
			double r = physics.random(0, physics.random(0, diskRadius));
			double depth = physics.random(0, physics.random(0, physics.random(0, diskRadius)));

			double armAngle = 2 * 3.14 * ((j - startIndex) * 1000 / particles.size()) / numArms;

			particles[j].position = glm::dvec3(r * std::cos(armAngle), r * std::sin(armAngle), physics.random(-depth, depth)) + position;
			double distanceToCenter = glm::length(particles[j].position - position);
			double v = std::sqrt((physics.G * blackHoleMass) / distanceToCenter);
			particles[j].velocity = glm::dvec3(-v * std::sin(armAngle), v * std::cos(armAngle), 0) + velocity;
			particles[j].mass = 1e30;
			particles[j].radius = physics.random(0.1, 2);
			particles[j].color = glm::vec3(1, 1, 1);
		}

		//dark matter in the outer regions
		else if (j >= darkMatterStart && j < darkMatterEnd)
		{
			double r = physics.random(physics.random(0, diskRadius), diskRadius);
			double depth = physics.random(0, physics.random(0, physics.random(0, diskRadius))) / 5;

			double angle = physics.random(0, 2 * 3.14);

			particles[j].position = glm::dvec3(r * std::cos(angle), r * std::sin(angle), physics.random(-depth, depth)) + position;
			double distanceToCenter = glm::length(particles[j].position - position);
			double v = std::sqrt((physics.G * blackHoleMass) / distanceToCenter);
			particles[j].velocity = glm::dvec3(-v * std::sin(angle), v * std::cos(angle), 0) + velocity;
			particles[j].mass = 1e30;
			particles[j].radius = physics.random(0.1, 2);
			particles[j].color = glm::vec3(1, 1, 1);
			particles[j].darkMatter = true;
		}
		i++;
	}
}