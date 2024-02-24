#include "SystemInit.h"
#include <iostream>
#include <cmath>
#include <random>
#include <gtc/quaternion.hpp>
#include <gtx/quaternion.hpp>
#include "FileManager.h"
#include <vector>
#include <string>
#include <sstream>
#include <map>
#include <iostream>
#include <filesystem>
#include <string>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include "MathFunctions.h"
#include "FileManager.h"
namespace fs = std::filesystem;

using namespace std;

void SystemInit::start(Physics* phy, std::vector<Particle>& particles)
{
	//manuelle Initialisierung
	//andromeda vorsimulation with 2 particles 
	if (false)
	{
		Particle particle1;
		particle1.position = glm::vec3(0, 0, 0);
		particle1.velocity = glm::vec3(0, 0, 0);
		particle1.mass = 2.9833800017169e42;
		particle1.radius = 10;
		particle1.color = glm::vec3(1, 1, 0);
		particle1.darkMatter = false;
		particles[0] = particle1;

		Particle particle2;
		particle2.position = glm::vec3(2.3841041e22, 0, 0);
		particle2.velocity = glm::vec3(-300000, 0, 0);
		particle2.mass = 2.7844880016024e42;
		particle2.radius = 10;
		particle2.color = glm::vec3(1, 1, 0);
		particle2.darkMatter = false;
		particles[1] = particle2;
	}
	else
	{
		Physics p;
		//Init nach Config File
		if (true)
		{
			// Einlesen der Konfigurationen
			FileManager* fileManager = new FileManager("config.ini");
			auto config = fileManager->readTheConfig("config.ini");

			// Verarbeiten der Galaxie-Konfigurationen
			int galaxieNummer = 1;
			std::string galaxieKey = "Galaxie" + std::to_string(galaxieNummer);
			int startIndex = 1;

			while (config.find(galaxieKey + ".Typ") != config.end())
			{
				string typ = config[galaxieKey + ".Typ"];
				string hubbleklass = config[galaxieKey + ".HubbleKlassifikation"];
				int particlesize = std::stoi(config[galaxieKey + ".ParticleSize"]);
				double radius = std::stod(config[galaxieKey + ".Radius"]);
				double Masse = std::stod(config[galaxieKey + ".GesammtMasse"]);
				double anteilBaryonischeMaterie = std::stod(config[galaxieKey + ".AnteilBaryonischeMaterie"]);
				double anteilDunkleMaterie = std::stod(config[galaxieKey + ".AnteilDunkleMaterie"]);
				double powNumberNormal = std::stod(config[galaxieKey + ".VerteilungsExponentBaryonischeMaterie"]);
				double powNumberDark = std::stod(config[galaxieKey + ".VerteilungsExponentDunkleMaterie"]);

				// Position, Geschwindigkeit, Rotation
				glm::dvec3 position = fileManager->parseVector3(config[galaxieKey + ".Position"]);
				glm::dvec3 velocity = fileManager->parseVector3(config[galaxieKey + ".Geschwindigkeit"]);
				glm::dvec3 rotation = fileManager->parseVector3(config[galaxieKey + ".Rotation"]);

				// scale all the values to the right units
				radius = (double)phy->units->lengthUnit / radius;
				Masse = (double)phy->units->massUnit / Masse;
				position = position * (1 / (double)phy->units->lengthUnit);
				velocity = velocity * (1 / (double)phy->units->velocityUnit);
				rotation = rotation * (1 / (double)phy->units->velocityUnit);

				int endIndex = startIndex + particlesize - 1;

				if (typ == "Spiral")
				{
					if (hubbleklass == "Sa")
					{
						//spiral galaxie with theese parameter:int startIndex, int endIndex, glm::dvec3 position, glm::dvec3 rotation, glm::dvec3 velocity, double maxRadius, double Masse, double anteilBaryonischeMaterie, double anteilDunkleMaterie, double powNumberNormal, double powNumberDark, std::vector<Particle>& particles
						spiralGalaxy.Sa(startIndex, endIndex, position, rotation, velocity, radius, Masse, anteilBaryonischeMaterie, anteilDunkleMaterie, powNumberNormal, powNumberDark, particles);
					}
					if (hubbleklass == "Sb")
					{
						spiralGalaxy.Sb(startIndex, endIndex, position, rotation, velocity, radius, Masse, anteilBaryonischeMaterie, anteilDunkleMaterie, powNumberNormal, powNumberDark, particles);
					}
					if (hubbleklass == "Sc")
					{
						spiralGalaxy.Sc(startIndex, endIndex, position, rotation, velocity, radius, Masse, anteilBaryonischeMaterie, anteilDunkleMaterie, powNumberNormal, powNumberDark, particles);
					}
				}
				if (typ == "Elliptisch")
				{
					if (hubbleklass == "E0")
					{
						ellipticalGalaxy.E0(startIndex, endIndex, position, rotation, velocity, radius, Masse, anteilBaryonischeMaterie, anteilDunkleMaterie, powNumberNormal, powNumberDark, particles);
					}
					if (hubbleklass == "S0")
					{
						ellipticalGalaxy.S0(phy, startIndex, endIndex, position, rotation, velocity, radius, Masse, anteilBaryonischeMaterie, anteilDunkleMaterie, powNumberNormal, powNumberDark, particles);
					}
				}
				startIndex = endIndex; // Aktualisieren des Startindex für die nächste Galaxie
				galaxieNummer++;
				galaxieKey = "Galaxie" + std::to_string(galaxieNummer);
			}
		}
		//manuelles Initialisieren
		else
		{
			//planet systemns without SPH

			//solarSystem(particles);
			//ourSolarSystem(particles);

			//galaxies

			//ellipticalGalaxy.E0(0, 19999, { 0,0,0 }, { 0,0,0 }, { 0,0,0 }, 1, 1e12, 0.5, 0.5, 0.5, 0.5, particles);
			//ellipticalGalaxy.S0(1, 9999, { 0,0,0 }, { 0,0,0 }, { 0,0,0 }, 1, particles);


			//Spiral galaxy like milky way
			//spiralGalaxy.Sa(1, 19999, { 1e22,0,0 }, { 0,0,0 }, { 0,0,0 }, 1e21, 1e42, 0.5, 0.5, 0.5, 0.5, particles);
			//ellipticalGalaxy.S0(10000, 19999, { 1.065e21, 0.565e21,0 }, { 0, 0, 0 }, { 0,0,0 }, 0.3, particles);
			//spiralGalaxy.Sc(20000, 29999, { 1e22,0,0 }, { 1,2,1 }, { 0,0,0 }, 0.5, particles);

			//barredGalaxy.SBa(0, 9999, { 0,0,0 }, { 0,0,0 }, { 0,0,0 }, 1, particles);
			//barredGalaxy.SBb(10000, 19999, { 1e22,0,0 }, { 0,0,0 }, { 0,0,0 }, 1, particles);
			//barredGalaxy.SBc(10000, 19999, { 1e22,0,0 }, { 0,0,0 }, { 0,0,0 }, 1, particles);
		}
	}
}

// solar system stuff 

void SystemInit::solarSystem(std::vector<Particle>& particles)
{
	Physics physics;
	double distanceFaktor = 1.4848e11;

	for (int j = 0; j < particles.size(); j++)
	{
		if (j == 0)
		{
			particles[j].position = glm::vec3(0, 0, 0);
			particles[j].velocity = glm::vec3(0, 0, 0);
			particles[j].mass = 5.972e30;
			particles[j].radius = 6;
			particles[j].color = glm::vec3(1, 1, 0);
		}
		else
		{
			double x = j * distanceFaktor;
			double v = std::sqrt((physics.G * particles[0].mass) / (x));
			particles[j].position = glm::dvec3(x, 0, 0);
			particles[j].velocity = glm::vec3(0, 0, 0);
			particles[j].mass = 5.972e25;
			std::cout << particles[j].velocity.y << std::endl;
			particles[j].radius = 3;
			particles[j].color = glm::vec3(MathFunctions::random(0,1), MathFunctions::random(0, 1), MathFunctions::random(0, 1));
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
			particle.mass = 1.898e20;
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
			particle.mass = 5.683e20;
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