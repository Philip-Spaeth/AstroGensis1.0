#include "SystemInit.h"
#include <iostream>
#include <cmath>
#include <random>
#include <gtc/quaternion.hpp>
#include <gtx/quaternion.hpp>

void SystemInit::start(std::vector<Particle>& particles)
{
	//solarSystem(particles);
	//ourSolarSystem(particles);

	//ellipticalGalaxy(0, 19999, { 0,0,0 }, { 0,300,0 }, { 0,0,0 }, particles);
	//ellipticalGalaxy(1000, 1999, { 2e22, 0, 0}, { 90,0,180 }, { 0,0,0 } , particles);
	
	spiralGalaxy(0, 19999, { 0,0,0 }, { 0, 0, 0}, { 0,0,0 }, particles);
	//spiralGalaxy(3000, 3999, { 1.5e22, 0.5e22, 0 }, { 234,30,129 }, { 0,0,0 }, particles);
}

void SystemInit::spiralGalaxy(int startIndex, int endIndex, glm::dvec3 position, glm::dvec3 rotation, glm::dvec3 velocity, std::vector<Particle>& particles)
{
	int size = endIndex + 1 - startIndex; 

	double diskRadius = 1e21;
	double BulgeRadius = 3e20;

	int numArms = 2;
	int i = 0;

	//verteillung der masse 
	double totalMass = 1e42;

	double percentBulge = 0.3;
	double bulgeMass = percentBulge * totalMass;

	double percentDisk = 0.5;
	double diskMass = percentDisk * totalMass;

	double percentDarkMatter = 0.2;
	double darkMatterMass = percentDarkMatter * totalMass;
	double haloRadius = 1e22;


	//split the size of the galaxy in 3 parts
	int bulgeSize = size * percentBulge;
	int diskSize = size * percentDisk;
	int darkMatterSize = size * percentDarkMatter;

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
				particles[j].mass = 1e36;
				particles[j].radius = 0.5;
				particles[j].color = glm::vec3(1, 1, 1);
			}
			else
			{
				double r = physics.random(0, physics.random(0, BulgeRadius));
				double depth = physics.random(0, physics.random(0, physics.random(0, BulgeRadius)));

				double angle = physics.random(0, 2 * 3.14);

				double v = std::sqrt((physics.G * 1e36) / r);
				particles[j].position = glm::dvec3(r * std::cos(angle), r * std::sin(angle), physics.random(-depth, depth)) + position; 
				particles[j].velocity = glm::dvec3(-v * std::sin(angle), v * std::cos(angle), 0) + velocity;
				particles[j].mass = 1e30;
				particles[j].radius = physics.random(0.001, 2);
				particles[j].color = glm::vec3(1, 1, 1);
			}
		}
		//disk 
		else if (j >= diskStart && j < diskEnd)
		{
			double r = physics.random(0, physics.random(0, diskRadius));
			double depth = physics.random(0, physics.random(0, physics.random(0, diskRadius)));

			double armAngle = 2 * 3.14 * ((j - startIndex) * 1000 / particles.size()) / numArms;

			double v = std::sqrt((physics.G * 1e36) / r);
			particles[j].position = glm::dvec3(r * std::cos(armAngle), r * std::sin(armAngle), physics.random(-depth, depth)) + position;
			particles[j].velocity = glm::dvec3(-v * std::sin(armAngle), v * std::cos(armAngle), 0) + velocity;
			particles[j].mass = 1e30;
			particles[j].radius = physics.random(0.001, 2);
			particles[j].color = glm::vec3(1, 1, 1);
		}

		//dark matter in the outer regions
		else if (j >= darkMatterStart && j < darkMatterEnd)
		{
			double r = physics.random(physics.random(0, diskRadius), diskRadius);
			double depth = physics.random(0, physics.random(0, physics.random(0, diskRadius))) / 5;

			double angle = physics.random(0, 2 * 3.14);

			double v = std::sqrt((physics.G * 1e36) / r);
			particles[j].position = glm::dvec3(r * std::cos(angle), r * std::sin(angle), physics.random(-depth, depth)) + position;
			particles[j].velocity = glm::dvec3(-v * std::sin(angle), v * std::cos(angle), 0) + velocity;
			particles[j].mass = 1e34;
			particles[j].radius = physics.random(0.001, 2);
			particles[j].color = glm::vec3(1, 1, 1);
			particles[j].darkMatter = true;
		}
		i++;
	} 
}	

void SystemInit::ellipticalGalaxy(int startIndex, int endIndex, glm::dvec3 position, glm::dvec3 rotation, glm::dvec3 velocity, std::vector<Particle>& particles)
{
	int size = endIndex + 1;

	double galaxyRadius = 1e21; // Radius der kugelförmigen Galaxie

	double starSpeed = 1;

	int i = 0;

	// Erstellen einer kugelförmigen Galaxie
	for (int j = startIndex; j < size; j++)
	{
		// Mass Sagittarius A
		if (j == startIndex)
		{
			particles[j].position = position;
			particles[j].velocity = velocity;
			particles[j].mass = 1e36;
			particles[j].radius = 0.5;
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

			particles[j].position = glm::dvec3(x, y, z) + position;
			double v = std::sqrt((physics.G * particles[startIndex].mass) / particles[startIndex].CalculateDistance(particles[j])) * starSpeed;
			particles[j].velocity = glm::dvec3(-v * std::sin(phi), v * std::cos(phi), 0) + velocity;
			particles[j].mass = 1e30;
			particles[j].radius = physics.random(0.001,2);
			//make the stars in the center brighter
			double brightness = 1 - (particles[j].CalculateDistance(particles[startIndex]) / galaxyRadius);
			particles[j].color = glm::vec3(brightness - 0.1, brightness - 0.1, brightness);
		}

		i++;
	}
}


void SystemInit::solarSystem(std::vector<Particle>& particles)
{
	Physics physics;
	double distanceFaktor = 1.4848e11;

	for (int j = 0; j < physics.particlesSize; j++)
	{
		if (j == 0)
		{
			particles[j].position = glm::vec3(0, 0, 0);
			particles[j].velocity = glm::vec3(0, 0, 0);
			particles[j].mass = 1.989e30;
			particles[j].radius = 6;
			particles[j].color = glm::vec3(1, 1, 0);
		}
		else
		{
			double x = j * distanceFaktor;
			double v = std::sqrt((physics.G * particles[0].mass) / (x));
			particles[j].position = glm::dvec3(x, 0, 0);
			particles[j].velocity = glm::vec3(0, v, 0);
			particles[j].mass = 5.972e24;
			std::cout << particles[j].velocity.y << std::endl;
			particles[j].radius = 3;
			particles[j].color = glm::vec3(physics.random(0,1), physics.random(0, 1), physics.random(0, 1));
		}
	}
}

void SystemInit::ourSolarSystem(std::vector<Particle>& particles)
{
	// Data from JPL Horizons Nasa date: 2020-01-01 https://ssd.jpl.nasa.gov/horizons/app.html#/

	Physics physics;

	for (int j = 0; j < physics.particlesSize; j++)
	{
		//barycenter
		if (j == 0)
		{
			Particle particle;
			particle.position = glm::vec3(0, 0, 0);
			particle.velocity = glm::vec3(0, 0, 0);
			particle.mass = 0;
			particle.radius = 0.5;
			particle.color = glm::vec3(1, 1, 1);
			particles[j] = particle;
		}

		//Sun
		if (j == 1)
		{
			Particle particle;
			particle.position = glm::vec3(-5.683718842782438E+08, 1.112952208755958E+09, 3.459173372169666E+06);
			particle.velocity = glm::vec3(-1.446153155910682E-05, -3.447513466324272E-06, 3.988450015258638E-07);
			particle.mass = 1.989e30;
			particle.radius = 6;
			particle.color = glm::vec3(1, 1, 0);
			particles[j] = particle;
		}

		//Mercury
		if (j == 2)
		{
			Particle particle;
			particle.position = glm::vec3(-1.004313454665499E+010, -6.782852744259514E+010, -4.760875668975301E+09);
			particle.velocity = glm::vec3(3.847265152100766E+04, 4.158689617302953E+03, -3.869763814829368E+03);
			particle.mass = 3.285e23;
			particle.radius = 3;
			particle.color = glm::vec3(1, 1, 1);
			particles[j] = particle;
		}

		//Venus
		if (j == 3)
		{
			Particle particle;
			particle.position = glm::vec3(1.076208530906284E+011, 8.974077731382189E+09, -6.131962732360962E+09);
			particle.velocity = glm::vec3(-2.693485036366443E+03, 3.476650461674002E+04, 6.320912270841283E-04);
			particle.mass = 4.867e24;
			particle.radius = 3;
			particle.color = glm::vec3(1, 1, 1);
			particles[j] = particle;
		}

		//Earth
		if (j == 4)
		{
			Particle particle;
			particle.position = glm::vec3(-2.545334341413143E+010, 1.460912993606863E+011, -2.712536257669330E+06);
			particle.velocity = glm::vec3(-2.986338200299215E+04, -5.165822218319680E+03, 1.135505005904092E-06);
			particle.mass = 5.972e24;
			particle.radius = 3;
			particle.color = glm::vec3(0, 0, 1);
			particles[j] = particle;
		}

		//Mars
		if (j == 5)
		{
			Particle particle;
			particle.position = glm::vec3(-1.980536587315298E+11, -1.313944783612352E+11, 2.072259408826552E+09);
			particle.velocity = glm::vec3(1.439273929147519E+04, -1.805004075338039E+04, -7.312486307777304E-04);
			particle.mass = 6.39e23;
			particle.radius = 3;
			particle.color = glm::vec3(1, 0, 0);
			particles[j] = particle;
		}

		//Jupiter
		if (j == 6)
		{
			Particle particle;
			particle.position = glm::vec3(7.814210740177372E+10, -7.769489405106664E+11, 1.474081608655989E+09);
			particle.velocity = glm::vec3(1.283931035365873E+04, 1.930357075733133E+03, -2.952599466798547E-04);
			particle.mass = 1.898e27;
			particle.radius = 3;
			particle.color = glm::vec3(194, 152, 0);
			particles[j] = particle;
		}

		//Saturn
		if (j == 7)
		{
			Particle particle;
			particle.position = glm::vec3(5.674913734259648E+11, -1.388366508315161E+12, 1.549279065912783E+09);
			particle.velocity = glm::vec3(8.406493534983737E+03, 3.627774825767542E+03, -1.778058503736513E-04);
			particle.mass = 5.683e26;
			particle.radius = 3;
			particle.color = glm::vec3(194, 152, 0);
			particles[j] = particle;
		}

		//Uranus
		if (j == 8)
		{
			Particle particle;
			particle.position = glm::vec3(2.426731459899411E+012, 1.703392613402010E+12, -2.511227721322751E+10);
			particle.velocity = glm::vec3(-3.962351347249518E+03, 5.256523406016862E+03, 7.095188573163291E-05);
			particle.mass = 8.681e25;
			particle.radius = 3;
			particle.color = glm::vec3(150, 150, 150);
			particles[j] = particle;
		}

		//Neptune
		if (j == 9)
		{
			Particle particle;
			particle.position = glm::vec3(4.374094185858724E+12, -9.514046294520888E+011, -8.121285263483042E+010);
			particle.velocity = glm::vec3(1.119118833334523E+03, 5.343445550714678E+03, -1.357620460013313E-04);
			particle.mass = 1.024e26;
			particle.radius = 3;
			particle.color = glm::vec3(0, 0, 1);
			particles[j] = particle;
		}
	}
}