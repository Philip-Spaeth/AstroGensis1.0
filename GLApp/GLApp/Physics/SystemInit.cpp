
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
<<<<<<< Updated upstream
=======
#include <iostream>
#include <filesystem>
#include <string>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
namespace fs = std::filesystem;
>>>>>>> Stashed changes

using namespace std;

// Hilfsfunktion zum Parsen von glm::vec3 aus einem String
glm::vec3 parseVector3(const std::string& vecString) {
	std::istringstream iss(vecString);
	std::vector<double> values;
	std::string s;
	while (getline(iss, s, ',')) {
		values.push_back(std::stod(s));
	}
	return glm::vec3(values[0], values[1], values[2]);
}

std::unordered_map<std::string, std::string> readTheConfig(const std::string& filename)
{
<<<<<<< Updated upstream
	Physics p;
	//Init nach Config File
	if (p.configFile)
	{
		FileManager*fileManager = new FileManager("");
		auto galaxyConfigs = fileManager->parseIniFileBySection("config.ini");

		for (auto& config : galaxyConfigs) {
			createGalaxy(config, particles);
		}
	}
	//manuelles Initialisieren
	else
	{
		//planet systemns without SPH
		//solarSystem(particles);
		//ourSolarSystem(particles);

		//galaxies
		//ellipticalGalaxy.E0(1, 9999, { 0,0,0 }, { 0,0,0 }, {0,0,0}, 1, particles);
		ellipticalGalaxy.S0(1, 9999, { 0,0,0 }, { 0,0,0 }, { 0,0,0 }, 1, particles);

		//spiralGalaxy.Sb(1, 9999, { 0,0,0 }, { 0,0,0 }, { 0,0,0 }, 1, particles);
		//ellipticalGalaxy.S0(10000, 19999, { 1.065e21, 0.565e21,0 }, { 0, 0, 0 }, { 0,0,0 }, 0.3, particles);
		//spiralGalaxy.Sc(20000, 29999, { 1e22,0,0 }, { 1,2,1 }, { 0,0,0 }, 0.5, particles);

		//barredGalaxy.SBa(0, 9999, { 0,0,0 }, { 0,0,0 }, { 0,0,0 }, 1, particles);
		//barredGalaxy.SBb(10000, 19999, { 1e22,0,0 }, { 0,0,0 }, { 0,0,0 }, 1, particles);
		//barredGalaxy.SBc(10000, 19999, { 1e22,0,0 }, { 0,0,0 }, { 0,0,0 }, 1, particles);
	}
=======
	std::unordered_map<std::string, std::string> config;
	std::ifstream file(filename);
	std::string line;
	std::string currentSection;

	if (file.is_open()) {
		while (std::getline(file, line)) {
			// Entfernen Sie Leerzeichen am Anfang der Zeile
			line.erase(0, line.find_first_not_of(" \t"));

			// Erkenne Abschnittsnamen
			if (line[0] == '[') {
				size_t endPos = line.find(']');
				if (endPos != std::string::npos) {
					currentSection = line.substr(1, endPos - 1) + ".";
				}
				continue;
			}

			// Entferne Kommentare am Ende der Zeile
			size_t commentPos = line.find_first_of("#;");
			if (commentPos != std::string::npos) {
				line = line.substr(0, commentPos);
			}

			std::istringstream is_line(line);
			std::string key;
			if (std::getline(is_line, key, '=')) {
				std::string value;
				if (std::getline(is_line, value)) {
					// Entfernen Sie Leerzeichen am Anfang und am Ende des Schlüssels und des Werts
					key.erase(0, key.find_first_not_of(" \t"));
					key.erase(key.find_last_not_of(" \t") + 1);
					value.erase(0, value.find_first_not_of(" \t"));
					value.erase(value.find_last_not_of(" \t") + 1);
					config[currentSection + key] = value;
				}
			}
		}
		file.close();
	}
	else {
		std::cerr << "Konnte Datei nicht öffnen: " << filename << std::endl;
	}

	return config;
>>>>>>> Stashed changes
}

void SystemInit::createGalaxy(const std::map<std::string, std::string>& config, std::vector<Particle>& particles) {
	std::string type = config.at("Typ");
	std::string hubbleKlassifikation = config.at("HubbleKlassifikation");
	int particleSize = std::stoi(config.at("ParticleSize"));
	double radius = std::stod(config.at("Radius"));
	double gesammtMasse = std::stod(config.at("GesammtMasse"));
	double anteilBaryonischeMaterie = std::stod(config.at("AnteilBaryonischeMaterie"));
	double anteilDunkleMaterie = std::stod(config.at("AnteilDunkleMaterie"));

	// Position und Geschwindigkeit extrahieren
	std::vector<std::string> posTokens, velTokens, rotTokens;
	std::string token;
	std::istringstream posStream(config.at("Position"));
	std::istringstream velStream(config.at("Geschwindigkeit"));
	std::istringstream rotStream(config.at("Rotation"));

<<<<<<< Updated upstream
	while (std::getline(posStream, token, ',')) {
		posTokens.push_back(token);
	}
	while (std::getline(velStream, token, ',')) {
		velTokens.push_back(token);
	}
	while (std::getline(rotStream, token, ',')) {
		rotTokens.push_back(token);
	}

	glm::dvec3 position(std::stod(posTokens[0]), std::stod(posTokens[1]), std::stod(posTokens[2]));
	glm::dvec3 geschwindigkeit(std::stod(velTokens[0]), std::stod(velTokens[1]), std::stod(velTokens[2]));
	glm::dvec3 rotation(std::stod(rotTokens[0]), std::stod(rotTokens[1]), std::stod(rotTokens[2]));
=======
void SystemInit::start(std::vector<Particle>& particles)
{
	Physics p;
	//Init nach Config File
	if (p.configFile)
	{
		auto config = readTheConfig("config.ini");

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
			glm::vec3 position = parseVector3(config[galaxieKey + ".Position"]);
			glm::vec3 velocity = parseVector3(config[galaxieKey + ".Geschwindigkeit"]);
			glm::vec3 rotation = parseVector3(config[galaxieKey + ".Rotation"]);

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
					ellipticalGalaxy.S0(startIndex, endIndex, position, rotation, velocity, radius, Masse, anteilBaryonischeMaterie, anteilDunkleMaterie, powNumberNormal, powNumberDark, particles);
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
		spiralGalaxy.Sa(1, 19999, { 1e22,0,0 }, { 0,0,0 }, { 0,0,0 }, 1e21, 1e42, 0.5, 0.5, 0.5, 0.5, particles);
		//ellipticalGalaxy.S0(10000, 19999, { 1.065e21, 0.565e21,0 }, { 0, 0, 0 }, { 0,0,0 }, 0.3, particles);
		//spiralGalaxy.Sc(20000, 29999, { 1e22,0,0 }, { 1,2,1 }, { 0,0,0 }, 0.5, particles);

		//barredGalaxy.SBa(0, 9999, { 0,0,0 }, { 0,0,0 }, { 0,0,0 }, 1, particles);
		//barredGalaxy.SBb(10000, 19999, { 1e22,0,0 }, { 0,0,0 }, { 0,0,0 }, 1, particles);
		//barredGalaxy.SBc(10000, 19999, { 1e22,0,0 }, { 0,0,0 }, { 0,0,0 }, 1, particles);
	}
}
>>>>>>> Stashed changes

	// Hier erstellen Sie die Galaxie basierend auf dem Typ und den weiteren Parametern
	if (type == "Spiral") {
		// Erstellen Sie eine Spiralgalaxie
		// Beispiel: spiralGalaxy.createGalaxy(particleSize, radius, position, geschwindigkeit, rotation, gesammtMasse, anteilBaryonischeMaterie, anteilDunkleMaterie, particles);
	}
	else if (type == "Elliptisch") {
		// Erstellen Sie eine elliptische Galaxie
		// Beispiel: ellipticalGalaxy.createGalaxy(particleSize, radius, position, geschwindigkeit, rotation, gesammtMasse, anteilBaryonischeMaterie, anteilDunkleMaterie, particles);
	}
}

// solar system stuff 

void SystemInit::solarSystem(std::vector<Particle>& particles)
{
	Physics physics;
	double distanceFaktor = 1.4848e11;

	for (int j = 0; j < physics.particlesSize; j++)
	{
		if (j == 0)
		{
			particles[j].position = glm::vec3(0, -1.4848e11, 0);
			particles[j].velocity = glm::vec3(0, 0, 0);
			particles[j].mass = 5.972e26;
			particles[j].radius = 6;
			particles[j].color = glm::vec3(1, 1, 0);
		}
		else
		{
			double x = j * distanceFaktor;
			double v = std::sqrt((physics.G * particles[0].mass) / (x));
			particles[j].position = glm::dvec3(x, 0, 0);
			particles[j].velocity = glm::vec3(0, 0, 0);
			particles[j].mass = 5.972e26;
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