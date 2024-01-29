#pragma once

#include <glm.hpp>
#include "Particle.h"
#include <vector>
#include <fstream>
#include <iostream>
#include <string>
#include "Physics.h"


class FileManager
{
	public:
	FileManager(std::string dataFolder);
	~FileManager();

	std::string dataFolder = "Data";
	
	void saveParticles(int timestep, const std::vector<Particle>& particles, const std::string& path);
	void loadParticles(int timestep, std::vector<glm::vec4>& array, std::vector<glm::vec3>& color, std::vector<glm::vec3>& densitycolor);

	void saveEnergieData(std::vector<std::vector<double>>& totalEnergie, std::string path);

	//rotationCurve in galaxy, dark and visible matter
	void saveRotationCurve(std::vector<Particle>& particles, std::string path);

	//massCurve in galaxy, dark and visible matter
	void saveMassCurve(std::vector<Particle>& particles, std::string path);

	private:
};