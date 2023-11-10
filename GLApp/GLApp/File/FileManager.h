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
	FileManager();
	~FileManager();
	
	void saveParticles(int timestep, std::vector<Particle> &particles, std::string path);
	void loadParticles(std::vector<std::vector<Particle>>& particles);

	void saveEnergieData(std::vector<std::vector<double>>& totalEnergie, std::string path);

	private:
};