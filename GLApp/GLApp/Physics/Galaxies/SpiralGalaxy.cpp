#include "SpiralGalaxy.h"
#include <omp.h>
#include <iostream>
#include <cmath>
#include "MathFunctions.h"

void SpiralGalaxy::ellipticalOrbit(Particle& p, double m, double diskR, double r, double angle)
{
	double ellipticity = 0.1;
	double mainAxis = r;
	double minorAxis = r * (ellipticity + 1);

	double a = MathFunctions::random(0, 2 * 3.14);
	double x = mainAxis * std::cos(a);
	double y = minorAxis * std::sin(a);
	double z = 0;

	double bulgeScale = 0.05;
	double diskScale = 0.03;
	double bulgeR = diskR / 10;
	// Berechnen der Z-Koordinate basierend auf der Position im Bulge oder in der Disk
	z = MathFunctions::gaussianRandom() * 0.05 * diskR;
	double l = (diskR * 0.5) / r;
	if (l > 2)
	{
		l = 2;
	}
	z = z * l;

	/*
	//wenn in bulge rotier um die mitte der galaxie random
	if (r < bulgeR)
	{
		double random = MathFunctions::random(0, 2 * 3.14);
		x = x * std::cos(random) - y * std::sin(random);
		y = x * std::sin(random) + y * std::cos(random);
		z = z * std::cos(random) - y * std::sin(random);
	}
	*/

	//distanz zum zetrum berechnen
	double distanceToCenter = glm::abs(glm::length(glm::dvec3(x, y, 0)));
	//null ausschließen um division durch null zu vehindern
	if (distanceToCenter == 0)
	{
		distanceToCenter = 1;
	}

	// Setzen Sie die Position
	p.position = glm::dvec3(x * std::cos(angle) - y * sin(angle), x * std::sin(angle) + y * cos(angle), z);

///////////////////////// Velocity /////////////////////////
	double e = 1;

	//slwo down in bulge to cause caotic movement so its more realistic
	if (distanceToCenter < diskR)
	{
		e = ((distanceToCenter * distanceToCenter) / diskR) / diskR* (diskR / distanceToCenter);
		if (e > 1)
		{
			e = 1;
		}
	}
	// Berechnung der Geschwindigkeit
	double v = (std::sqrt(6.67430e-11 * m / distanceToCenter) * e) *(1 + ellipticity);

	//velocity tangent to the orbit 
	glm::dvec3 direction = glm::dvec3(-p.position.y, p.position.x, p.position.z);
	direction = glm::normalize(direction);
	p.velocity = direction * v;
}

void SpiralGalaxy::Sa(int startIndex, int endIndex, glm::dvec3 position, glm::dvec3 rotation, glm::dvec3 velocity, double maxRadius, double Masse, double anteilBaryonischeMaterie, double anteilDunkleMaterie, double powNumberNormal, double powNumberDark, std::vector<Particle>& particles)
{
	int particleSize = endIndex + 1 - startIndex;

	double galaxyRadius = maxRadius; // Radius der kugelförmigen Galaxie

	double totalMass = Masse; // Gesamtmasse der Galaxie
	double baryonicFraction = anteilBaryonischeMaterie; // Anteil der baryonischen Materie
	double darkMatterFraction = anteilDunkleMaterie; // Anteil der dunklen Materie

	double powNumber = powNumberNormal; // Verteilungsparameter für baryonische Materie
	double darkPowNumber = powNumberDark; // Verteilungsparameter für dunkle Materie

	int i = 0;

	glm::mat4 transform = glm::mat4(1.0f); // Identity matrix

	// Apply rotations
	transform = glm::rotate(transform, (float)rotation.x, glm::vec3(1, 0, 0));
	transform = glm::rotate(transform, (float)rotation.y, glm::vec3(0, 1, 0));
	transform = glm::rotate(transform, (float)rotation.z, glm::vec3(0, 0, 1));


	for (int j = startIndex; j != endIndex; j++)
	{
		double random = MathFunctions::random(-baryonicFraction, darkMatterFraction);
		bool isDarkMatterParticle;
		if (random > 0)
		{
			isDarkMatterParticle = true;
		}
		else
		{
			isDarkMatterParticle = false;
		}

		double scaledI = std::pow(static_cast<double>(j - startIndex) / particleSize, 1.0 / (isDarkMatterParticle ? darkPowNumber : powNumber));
		double r = galaxyRadius * scaledI;

		particles[j].angle = sqrt(r) * 2.0 * 3.14;

		//denisty wave angle with A and k
		double A = 1.8;
		double alpha = (i / (double)particleSize) * 2 * 3.14 * A;

		// Berechnung der Masse in der Kugel bis zum aktuellen Partikel
		double baryonicMassInSphere = baryonicFraction * totalMass * std::pow(r / galaxyRadius, powNumber);
		double darkMassInSphere = darkMatterFraction * totalMass * std::pow(r / galaxyRadius, darkPowNumber);
		double totalMassInSphere = baryonicMassInSphere + darkMassInSphere;

		ellipticalOrbit(particles[j], totalMassInSphere, galaxyRadius, r, alpha);

		particles[j].mass = totalMass / particleSize;
		particles[j].radius = 1;
		particles[j].darkMatter = isDarkMatterParticle;
		if (isDarkMatterParticle)particles[j].color = glm::vec3(0, 1000, 0);
		else particles[j].color = glm::vec3(1, 1, 1);
		i++;

		particles[j].position += position;

		glm::vec4 pos = glm::vec4(particles[j].position - position, 1.0);
		glm::vec4 vel = glm::vec4(particles[j].velocity, 1.0);

		pos = transform * pos;
		vel = transform * vel;

		particles[j].position = glm::dvec3(pos) + position;
		particles[j].velocity = glm::dvec3(vel) + velocity;
	}
}



void SpiralGalaxy::Sb(int startIndex, int endIndex, glm::dvec3 position, glm::dvec3 rotation, glm::dvec3 velocity, double maxRadius, double Masse, double anteilBaryonischeMaterie, double anteilDunkleMaterie, double powNumberNormal, double powNumberDark, std::vector<Particle>& particles)
{
	int particleSize = endIndex + 1 - startIndex;

	double galaxyRadius = maxRadius; // Radius der kugelförmigen Galaxie

	double totalMass = Masse; // Gesamtmasse der Galaxie
	double baryonicFraction = anteilBaryonischeMaterie; // Anteil der baryonischen Materie
	double darkMatterFraction = anteilDunkleMaterie; // Anteil der dunklen Materie

	double powNumber = powNumberNormal; // Verteilungsparameter für baryonische Materie
	double darkPowNumber = powNumberDark; // Verteilungsparameter für dunkle Materie

	int i = 0;

	glm::mat4 transform = glm::mat4(1.0f); // Identity matrix

	// Apply rotations
	transform = glm::rotate(transform, (float)rotation.x, glm::vec3(1, 0, 0));
	transform = glm::rotate(transform, (float)rotation.y, glm::vec3(0, 1, 0));
	transform = glm::rotate(transform, (float)rotation.z, glm::vec3(0, 0, 1));


	for (int j = startIndex; j != endIndex; j++)
	{
		double random = MathFunctions::random(-baryonicFraction, darkMatterFraction);
		bool isDarkMatterParticle;
		if (random > 0)
		{
			isDarkMatterParticle = true;
		}
		else
		{
			isDarkMatterParticle = false;
		}

		//exponetial
		double scaledI = std::pow(static_cast<double>(j - startIndex) / particleSize, 1.0 / (isDarkMatterParticle ? darkPowNumber : powNumber));
		//linear
		//double scaledI = (particleSize / j);
		double r = galaxyRadius * scaledI;

		particles[j].angle = sqrt(r) * 2.0 * 3.14;

		//denisty wave angle with A and k
		double A = 1.1;
		double alpha = (i / (double)particleSize) * 2 * 3.14 * A;

		// Berechnung der Masse in der Kugel bis zum aktuellen Partikel
		double baryonicMassInSphere = baryonicFraction * totalMass * std::pow(r / galaxyRadius, powNumber);
		double darkMassInSphere = darkMatterFraction * totalMass * std::pow(r / galaxyRadius, darkPowNumber);
		double totalMassInSphere = baryonicMassInSphere + darkMassInSphere;

		ellipticalOrbit(particles[j], totalMassInSphere, galaxyRadius, r, alpha);

		particles[j].mass = totalMass / particleSize;
		particles[j].radius = 1;
		particles[j].darkMatter = isDarkMatterParticle;
		if (isDarkMatterParticle)particles[j].color = glm::vec3(0, 1000, 0);
		else particles[j].color = glm::vec3(1, 1, 1);
		i++;

		particles[j].position += position;

		glm::vec4 pos = glm::vec4(particles[j].position - position, 1.0);
		glm::vec4 vel = glm::vec4(particles[j].velocity, 1.0);

		pos = transform * pos;
		vel = transform * vel;

		particles[j].position = glm::dvec3(pos) + position;
		particles[j].velocity = glm::dvec3(vel) + velocity;
	}
}

void SpiralGalaxy::Sc(int startIndex, int endIndex, glm::dvec3 position, glm::dvec3 rotation, glm::dvec3 velocity, double maxRadius, double Masse, double anteilBaryonischeMaterie, double anteilDunkleMaterie, double powNumberNormal, double powNumberDark, std::vector<Particle>& particles)
{
	int particleSize = endIndex + 1 - startIndex;

	double galaxyRadius = maxRadius; // Radius der kugelförmigen Galaxie

	double totalMass = Masse; // Gesamtmasse der Galaxie
	double baryonicFraction = anteilBaryonischeMaterie; // Anteil der baryonischen Materie
	double darkMatterFraction = anteilDunkleMaterie; // Anteil der dunklen Materie

	double powNumber = powNumberNormal; // Verteilungsparameter für baryonische Materie
	double darkPowNumber = powNumberDark; // Verteilungsparameter für dunkle Materie

	int i = 0;

	glm::mat4 transform = glm::mat4(1.0f); // Identity matrix

	// Apply rotations
	transform = glm::rotate(transform, (float)rotation.x, glm::vec3(1, 0, 0));
	transform = glm::rotate(transform, (float)rotation.y, glm::vec3(0, 1, 0));
	transform = glm::rotate(transform, (float)rotation.z, glm::vec3(0, 0, 1));


	for (int j = startIndex; j != endIndex; j++)
	{
		double random = MathFunctions::random(-baryonicFraction, darkMatterFraction);
		bool isDarkMatterParticle;
		if (random > 0)
		{
			isDarkMatterParticle = true;
		}
		else
		{
			isDarkMatterParticle = false;
		}

		double scaledI = std::pow(static_cast<double>(j - startIndex) / particleSize, 1.0 / (isDarkMatterParticle ? darkPowNumber : powNumber));
		double r = galaxyRadius * scaledI;

		particles[j].angle = sqrt(r) * 2.0 * 3.14;

		//denisty wave angle with A and k
		double A = 0.8;
		double alpha = (i / (double)particleSize) * 2 * 3.14 * A;

		// Berechnung der Masse in der Kugel bis zum aktuellen Partikel
		double baryonicMassInSphere = baryonicFraction * totalMass * std::pow(r / galaxyRadius, powNumber);
		double darkMassInSphere = darkMatterFraction * totalMass * std::pow(r / galaxyRadius, darkPowNumber);
		double totalMassInSphere = baryonicMassInSphere + darkMassInSphere;

		ellipticalOrbit(particles[j], totalMassInSphere, galaxyRadius, r, alpha);

		particles[j].position += position;

		particles[j].mass = totalMass / particleSize;
		particles[j].radius = 1;
		particles[j].darkMatter = isDarkMatterParticle;
		if (isDarkMatterParticle)particles[j].color = glm::vec3(0, 1000, 0);
		else particles[j].color = glm::vec3(1, 1, 1);
		i++;

		glm::vec4 pos = glm::vec4(particles[j].position - position, 1.0);
		glm::vec4 vel = glm::vec4(particles[j].velocity, 1.0);

		pos = transform * pos;
		vel = transform * vel;

		particles[j].position = glm::dvec3(pos) + position;
		particles[j].velocity = glm::dvec3(vel) + velocity;
	}
}