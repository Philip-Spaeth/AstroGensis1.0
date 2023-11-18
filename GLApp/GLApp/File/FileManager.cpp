 #include "FileManager.h"
#include <filesystem>
#include "Physics.h"
#include <cmath>

FileManager::FileManager(){}

FileManager::~FileManager(){}

void FileManager::saveParticles(int timestep,std::vector<Particle> &particles, std::string path)
{
    std::string dataFolder = "Data";
    std::filesystem::create_directory(dataFolder);

    // Save data to a file for this time step
    std::string fileName = "Data/Time_" + std::to_string(timestep) + ".dat";
    std::ofstream outfile(fileName, std::ios::binary);
    if (outfile.is_open()) 
    {
        for (int p = 0; p < particles.size(); ++p)
        {
            outfile.write(reinterpret_cast<const char*>(&particles[p]), sizeof(Particle));
        }
        outfile.close();
    }
    else {
        std::cerr << "Error opening file for writing: " << fileName << std::endl;
    }
}

void FileManager::loadParticles(std::vector<std::vector<Particle>>& particles)
{
    // Laden der Daten für die Darstellung
    std::cout << "loading data ..." << std::endl;

    Physics physics;

    for (int t = 0; t < physics.numTimeSteps; ++t) {
        std::string fileName = "Data/Time_" + std::to_string(t) + ".dat";
        std::ifstream infile(fileName, std::ios::binary);
        if (infile.is_open()) {
            particles[t].resize(physics.particlesSize);
            for (int p = 0; p < physics.particlesSize; ++p) 
            {
                Particle particle; // Erstellen eines tempor�ren Particle-Objekts
                infile.read(reinterpret_cast<char*>(&particle), sizeof(Particle));

                // Den Partikel zum Partikelvektor hinzuf�gen
                particles[t][p] = particle;
            }
            infile.close();
        }
        else {
            std::cerr << "Error opening: " << fileName << std::endl;
        }
    }
}
void FileManager::saveEnergieData(std::vector<std::vector<double>>& totalEnergie, std::string path) 
{

    std::string filename = path;
    std::ofstream outputFile;
    outputFile.open(filename, std::ios::out);

    if (!outputFile.is_open()) {
        std::cerr << "Fehler beim Öffnen der Datei!" << std::endl;
        return;
    }

    std::locale germanLocale("de_DE.utf8");
    outputFile.imbue(germanLocale);

    double startEnergy = 0;
    double endEnergy = 0;

    for (size_t i = 1; i < totalEnergie.size(); ++i) 
    {
        double energy = 0;

        for (int j = 0; j < totalEnergie[i].size(); j++) {
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


        // Set precision for output file
        outputFile << std::fixed << std::setprecision(20) << energy;

        if (i < totalEnergie.size() - 1) {
            outputFile << "\n";
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

    outputFile.close();
}
