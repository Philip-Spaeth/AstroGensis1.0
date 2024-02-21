#include "FileManager.h"
#include <filesystem>
#include "Physics.h"
#include <iostream>
#include <vector>
#include "Particle.h"
#include <cmath>
#include <future>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <string>
#include <iomanip>
#include <unordered_map>
#include <algorithm>

#ifdef WIN32
#include <windows.h>
#endif

#include <thread>
#include <algorithm>

using namespace std;

FileManager::FileManager(std::string newDataFolder){
    dataFolder = newDataFolder;
}

FileManager::~FileManager(){}

std::unordered_map<std::string, std::string> FileManager::readConfig(const std::string& filename)
{
    std::unordered_map<std::string, std::string> config;
    std::ifstream file(filename);
    std::string line;

    if (file.is_open()) {
        while (std::getline(file, line)) {
            // Ignoriere Kommentare und leere Zeilen
            if (line.empty() || line[0] == '#' || line[0] == ';') continue;

            std::istringstream is_line(line);
            std::string key;
            if (std::getline(is_line, key, '=')) {
                std::string value;
                if (std::getline(is_line, value)) {
                    // delete leading and trailing whitespaces from key and value
                    key.erase(0, key.find_first_not_of(" \t"));
                    key.erase(key.find_last_not_of(" \t") + 1);
                    value.erase(0, value.find_first_not_of(" \t"));
                    value.erase(value.find_last_not_of(" \t") + 1);
                    config[key] = value;
                }
            }
        }
        file.close();
    }
    else {
        std::cerr << "Konnte Datei nicht öffnen: " << filename << std::endl;
    }

    return config;
}

glm::vec3 FileManager::parseVector3(const std::string& vecString) {
    istringstream iss(vecString);
    vector<double> values;
    string s;

    while (getline(iss, s, ',')) {
        // Entferne Leerzeichen am Anfang und am Ende des Strings
        s.erase(0, s.find_first_not_of(" \t"));
        s.erase(s.find_last_not_of(" \t") + 1);

        try {
            values.push_back(stod(s));
        }
        catch (const std::invalid_argument& e) {
            cerr << "Ungültiges Format für Vektor: " << vecString << endl;
            throw;
        }
    }

    if (values.size() != 3) {
        cerr << "Ungültige Anzahl an Elementen für Vektor: " << vecString << endl;
        throw std::runtime_error("Vector parsing error");
    }

    return glm::vec3(values[0], values[1], values[2]);
}

std::unordered_map<std::string, std::string> FileManager::readTheConfig(const std::string& filename)
{
    std::unordered_map<std::string, std::string> config;
    std::ifstream file(filename);
    std::string line;
    std::string currentSection;

    if (file.is_open()) {
        while (std::getline(file, line)) {
            // Ignoriere Kommentare und leere Zeilen
            size_t commentPos = line.find_first_of("#;");
            if (commentPos != std::string::npos) {
                line = line.substr(0, commentPos);
            }
            if (line.empty()) continue;

            // Entferne Leerzeichen am Anfang der Zeile
            line.erase(0, line.find_first_not_of(" \t"));

            // Erkenne Abschnittsnamen
            if (line[0] == '[') {
                size_t endPos = line.find(']');
                if (endPos != std::string::npos) {
                    currentSection = line.substr(1, endPos - 1) + ".";
                }
                continue;
            }

            std::istringstream is_line(line);
            std::string key;
            if (std::getline(is_line, key, '=')) {
                std::string value;
                if (std::getline(is_line, value)) {
                    // Entferne Leerzeichen am Anfang und am Ende des Schlüssels und des Werts
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
}

void FileManager::saveParticles(int timestep, const std::vector<Particle>& particles, const std::string& path)
{

    Physics physics;

    dataFolder = path;
    std::filesystem::create_directory("Data/" + dataFolder);

    // Save data to a file for this time step
    std::string fileName = "Data/" + dataFolder + "/Time_" + std::to_string(timestep) + ".dat";
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

void FileManager::loadParticles(Physics* p, int timestep, std::vector<glm::vec4>& array, std::vector<glm::vec3>& color, std::vector<glm::vec3>& densitycolor, int maxNumberOfParticles)
{
    std::string fileName = "Data/" + p->dataFolder + "/Time_" + std::to_string(timestep) + ".dat";
    std::ifstream file(fileName, std::ios::binary);

    if (file.is_open()) {
        size_t size;
        file.read(reinterpret_cast<char*>(&size), sizeof(size));

        size_t particlesToRead = (size > maxNumberOfParticles) ? maxNumberOfParticles : size;

        array.resize(particlesToRead);
        color.resize(particlesToRead);
        densitycolor.resize(particlesToRead);

        file.read(reinterpret_cast<char*>(array.data()), particlesToRead * sizeof(glm::vec4));
        file.read(reinterpret_cast<char*>(color.data()), particlesToRead * sizeof(glm::vec3));
        file.read(reinterpret_cast<char*>(densitycolor.data()), particlesToRead * sizeof(glm::vec3));

        // Skip the remaining data in the file if necessary
        if (size > maxNumberOfParticles) {
            file.seekg((size - maxNumberOfParticles) * (sizeof(glm::vec4) + 2 * sizeof(glm::vec3)), std::ios::cur);
        }

        file.close();
    }
    else {
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
    std::cout << std::endl;
    std::cout << "saving density curve ..." << std::endl;
    std::string filename = "../density_Curve.txt";
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

                steps[k].density = particles[i].density;
                steps[k].darkMatterDensity = particles[i].darkMatterDensity;
                steps[k].baryonicDensity = particles[i].baryonicDensity;
            }
        }
    }
    //print out the velocity and the distance to the center of mass
    for (int i = 0; i < steps.size(); i++)
    {
        double distance = sqrt(pow(steps[i].position.x, 2) + pow(steps[i].position.y, 2) + pow(steps[i].position.z, 2));
        double density = steps[i].density;
        double darkMatterDensity = steps[i].darkMatterDensity;
        double baryonicDensity = steps[i].baryonicDensity;
        //write the data to the file use , instead of . for the decimal point
        outputFile << std::fixed << std::setprecision(20) << distance << ";" << density << ";" << darkMatterDensity << ";" << baryonicDensity << ";" << std::endl;
    }

    outputFile.close();
}