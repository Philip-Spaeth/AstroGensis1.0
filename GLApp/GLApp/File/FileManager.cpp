#include "FileManager.h"
#include <filesystem>
#include "Physics.h"
#include <iostream>
#include <vector>
#include "Particle.h"
#include <cmath>
#include <future>

#ifdef WIN32
#include <windows.h>
#endif

#include <thread>
#include <algorithm>

FileManager::FileManager(){}

FileManager::~FileManager(){}

void FileManager::saveParticles(int timestep, const std::vector<Particle>& particles, const std::string& path)
{
    std::string dataFolder = "Data";
    std::filesystem::create_directory(dataFolder);

    // Save data to a file for this time step
    std::string fileName = "Data/Time_" + std::to_string(timestep) + ".dat";
    std::ofstream file(fileName, std::ios::binary);

    //put the particle positions and radius in a the array
    std::vector<glm::vec4> array;
    array.resize(particles.size());
    //put the particle color and dark matter bool in a the array
    std::vector<glm::vec3> color;
    std::vector<glm::vec3> densitycolor;
    color.resize(particles.size());
    densitycolor.resize(particles.size());
    for (int i = 0; i < particles.size(); i++)
    {
        array[i] = glm::vec4(particles[i].position, particles[i].radius);
        color[i] = glm::vec3(particles[i].color);
        densitycolor[i] = glm::vec3(particles[i].densityColor);
    }

    if (file.is_open()) 
    {
        size_t size = array.size();
        file.write(reinterpret_cast<char*>(&size), sizeof(size));

        file.write(reinterpret_cast<const char*>(array.data()), size * sizeof(glm::vec4));
        file.write(reinterpret_cast<const char*>(color.data()), size * sizeof(glm::vec3));
        file.write(reinterpret_cast<const char*>(densitycolor.data()), size * sizeof(glm::vec3));

        file.close();
    }
    else {
        std::cerr << "Fehler beim Öffnen der Datei zum Speichern." << std::endl;
    }
}

void FileManager::loadParticles(int timestep, std::vector<glm::vec4>& array, std::vector<glm::vec3>& color, std::vector<glm::vec3>& densitycolor)
{
    std::string fileName = "Data/Time_" + std::to_string(timestep) + ".dat";
    std::ifstream file(fileName, std::ios::binary);

    if (file.is_open()) {
        size_t size;
        file.read(reinterpret_cast<char*>(&size), sizeof(size));

        array.resize(Physics::particlesSize);
        color.resize(Physics::particlesSize);
        densitycolor.resize(Physics::particlesSize);
        //load the positions and radius from the file
        file.read(reinterpret_cast<char*>(array.data()), size * sizeof(glm::vec4));
        //load the color and dark matter bool from file
        file.read(reinterpret_cast<char*>(color.data()), size * sizeof(glm::vec3));
        //load the color and dark matter bool from file
        file.read(reinterpret_cast<char*>(densitycolor.data()), size * sizeof(glm::vec3));


        file.close();
    }
    else 
    {
        std::cerr << "Fehler beim Öffnen der Datei zum Laden." << std::endl;
    }
}


void FileManager::saveEnergieData(std::vector<std::vector<double>>& totalEnergie, std::string path) 
{
    double startEnergy = 0;
    double endEnergy = 0;

    for (size_t i = 0; i < totalEnergie.size(); ++i) 
    {
        double energy = 0;

        for (int j = 0; j < totalEnergie[i].size(); j++) 
        {
            energy += totalEnergie[i][j];
        }

        if (i == 1)
        {
            startEnergy = energy;
		}

        if (i == totalEnergie.size() - 1)
        {
            endEnergy = energy;
        }
    }
    double lostEnergy =startEnergy - endEnergy;

    std::cout << std::endl;
    std::cout << 1 << ": " << std::setprecision(20) << "Start Energy : " << startEnergy << std::endl;
    std::cout << totalEnergie.size() <<": " << std::setprecision(20) << "End Energy : " << endEnergy << std::endl;
    std::cout << std::endl;

    

    if (abs(startEnergy) > abs(endEnergy))
    {
        std::cout << std::setprecision(20) << "Lost Energy: " << lostEnergy << std::endl;
        std::cout << std::setprecision(10) << "or : " << (abs(lostEnergy) / abs(startEnergy)) * 100 << "%" << std::endl;
    }
    else
    {
        std::cout << std::setprecision(20) << "Added Energy: " << lostEnergy << std::endl;
        std::cout << std::setprecision(10) << "More energy than before: " << (lostEnergy / startEnergy) * 100 << "%" << std::endl;
    }
}

void FileManager::saveRotationCurve(std::vector<Particle>& particles, std::string path)
{
    std::cout << std::endl;
    std::cout << "saving velocity curve ..." << std::endl;
    std::string filename = "../velocty_Curve.txt";
    std::ofstream outputFile;
    outputFile.open(filename, std::ios::out);

    if (!outputFile.is_open()) {
        std::cerr << "Fehler beim Öffnen der Datei!" << std::endl;
        return;
    }

    std::locale germanLocale("de_DE.utf8");
    outputFile.imbue(germanLocale);

    double galaxySize = 1e21;

    int numberOfSteps = 1000;
    double binSize = galaxySize / numberOfSteps;

    std::vector<Particle> steps;
    steps.resize(numberOfSteps);

    //choose the particles that the closest current step
    //print out the velocity and the distance to the center of mass
    for (int k = 0; k < steps.size(); k++)
    {
        for (int i = 0; i < particles.size(); i++)
        {
            //clac the current distance from the step[k] to the current step 
            double distance = sqrt(pow(steps[k].position.x, 2) + pow(steps[k].position.y, 2) + pow(steps[k].position.z, 2));
            double delta = abs(distance - k * binSize);

            //calc the distance from the particle to the current step
            double particleDistance = sqrt(pow(particles[i].position.x, 2) + pow(particles[i].position.y, 2) + pow(particles[i].position.z, 2));
            double particleDelta = abs(particleDistance - k * binSize);

            //check if the particle is in the current step
            if (particleDelta < delta)
			{
				steps[k].position.x = particles[i].position.x;
				steps[k].position.y = particles[i].position.y;
				steps[k].position.z = particles[i].position.z;

				steps[k].velocity.x = particles[i].velocity.x;
				steps[k].velocity.y = particles[i].velocity.y;
				steps[k].velocity.z = particles[i].velocity.z;
			}
        }
    }
    //print out the velocity and the distance to the center of mass
	for (int i = 0; i < steps.size(); i++)
	{
		double distance = sqrt(pow(steps[i].position.x, 2) + pow(steps[i].position.y, 2) + pow(steps[i].position.z, 2));
        double velocity = sqrt(pow(steps[i].velocity.x, 2) + pow(steps[i].velocity.y, 2) + pow(steps[i].velocity.z, 2));
        //write the data to the file use , instead of . for the decimal point
        outputFile << std::fixed << std::setprecision(20) << distance << ";" << velocity << ";" << std::endl;
	}

    outputFile.close();
}

void FileManager::saveMassCurve(std::vector<Particle>& particles, std::string path)
{
    std::cout << "saving mass curve ..." << std::endl;
    std::string filename = "../mass_Curve.txt";
    std::ofstream outputFile;
    outputFile.open(filename, std::ios::out);

    if (!outputFile.is_open()) {
        std::cerr << "Fehler beim Öffnen der Datei!" << std::endl;
        return;
    }

    std::locale germanLocale("de_DE.utf8");
    outputFile.imbue(germanLocale);

    // Assuming you have a vector of radii
    std::vector<double> radii;

    // Extract radii from particles
    for (const auto& particle : particles) 
    {
        double distanceToCenter = glm::length(particle.position);
        radii.push_back(distanceToCenter);
    }

    // Sort the radii
    std::sort(radii.begin(), radii.end());

    // Remove duplicate radii
    radii.erase(std::unique(radii.begin(), radii.end()), radii.end());

    // Write header
    outputFile << "Radius\tTotalMass" << std::endl;

    // Calculate and write total mass for each radius
    for (const auto& radius : radii) 
    {
        double totalMass = 0.0;
        double darkMatterMass = 0.0;

        for (const auto& particle : particles) 
        {
            double distanceToCenter = glm::length(particle.position);

            if (distanceToCenter <= radius && particle.darkMatter != true) 
            {
                totalMass += particle.mass;
            }
            if (distanceToCenter <= radius && particle.darkMatter == true)
            {
                darkMatterMass += particle.mass;
            }
        }

        // Write data to the file
        //double p = totalMass / ((static_cast<double>(3) / 4) * 3.14 * (radius * radius * radius));
        //print out the mass and the radius
        outputFile << std::fixed << std::setprecision(2) << radius << ";" << totalMass << ";" << darkMatterMass << std::endl;
    }
    outputFile.close();
}