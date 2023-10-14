#define _USE_MATH_DEFINES
#include "Physics.h"
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <cmath>
#include <glm.hpp>
#include <gtc/constants.hpp>
#include <fstream>
#include "SystemInit.h"

Physics::Physics()
{
    // Initialisieren des Zufallszahlengenerators
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
}

bool Physics::Calc(std::vector<std::vector<Particle>>& particles)
{
    // Nur neu berechnen, wenn Enter gedrückt wird
    std::cout << "Press enter to start the calculations and press o to use render the old calculations" << std::endl;

    std::string input;
    std::getline(std::cin, input);

    if (input == "o")
    {
        return false;
    }

    std::cout << "Starting the calculations..." << std::endl;

    double time = numTimeSteps * ((particlesSize * timepercalc* particlesSize));
    //printing out the estimated time in seconds, minutes and hours, and days
    if (time < 60)
    {
		std::cout << "Estimated time: " << time << " seconds" << std::endl;
	}
    else if (time < 3600)
    {
		std::cout << "Estimated time: " << time / 60 << " minutes" << std::endl;
	}
    else if (time < 86400)
    {
		std::cout << "Estimated time: " << time / 3600 << " hours" << std::endl;
	}
    else
    {
		std::cout << "Estimated time: " << time / 86400 << " days" << std::endl;
	}
    

    std::string dataFolder = "Data";
    std::filesystem::create_directory(dataFolder);

    for (int t = 0; t < numTimeSteps; ++t) 
    {
        particles[t].resize(particlesSize);

        //Start values of the particles
        if (t == 0)
        {
            systemInit = new SystemInit();
            systemInit->start(particles);

            // Save data to a file for this time step
            std::string fileName = "Data/particles_" + std::to_string(t) + ".dat";
            std::ofstream outfile(fileName, std::ios::binary);
            if (outfile.is_open()) {
                for (int p = 0; p < particlesSize; ++p) {
                    outfile.write(reinterpret_cast<const char*>(&particles[t][p]), sizeof(Particle));
                }
                outfile.close();
            }
            else {
                std::cerr << "Error opening file for writing: " << fileName << std::endl;
            }
        }

        else
        {
            for (int p = 0; p < particlesSize; ++p)
            {
                Particle& currentParticle = particles[t][p];
                const Particle& previousParticle = particles[t - 1][p];

                // Übernehmen der alten Werte
                currentParticle.position = previousParticle.position;
                currentParticle.velocity = previousParticle.velocity;
                currentParticle.mass = previousParticle.mass;
                currentParticle.radius = previousParticle.radius;
                currentParticle.color = previousParticle.color;

                for (int i = 0; i < particlesSize; i++)
                {

                    if (i == p) {
                        continue;
                    }

                    //collision of black holes
                    if (particles[t][p].mass > 1000 && particles[t][i].mass > 1000 && particles[t][p].CalculateDistance(particles[t][i]) < 20)
                    {
                        particles[t][p].mass += particles[t][i].mass;
                        particles[t][i].mass = 0.00000001;
                        particles[t][i].radius = 0.0001;
                        particles[t][i].position.x = 1e20;
                    }


                    // Berechnen der neuen Werte mit dem Gravitationsgesetz von Newton
                    Particle& otherParticle = particles[t][i];

                    // Physik
                    double m1 = currentParticle.mass;
                    double m2 = otherParticle.mass;

                    glm::vec3 force = currentParticle.CalculateGravitationalForce(otherParticle, G, softening, faktor);

                    // Berechnen der neuen Position
                    currentParticle.UpdatePosition(faktor); // Übergeben Sie den Zeitschritt (deltaTime), den Sie hier verwenden möchten

                    calulations++;
                }
                //just for color not effecient
                if (currentParticle.colorMode == true)
                {
                    if (currentParticle.mass < 1000)
                    {
                        float gravityForce = particles[t][p].bigestGravitation;

                        if (gravityForce > highestForce)
                        {
                            if (particles[t][p].mass < 1000)
                            {
                                highestForce = gravityForce;
                            }
                        }

                        float color = gravityForce * 5 / highestForce;
                        std::cout << color << std::endl;
                        particles[t][p].color = glm::vec3(color/2 + 0.5, 0, color + 0.5);
                    }
                }
            }

            highestForce = 0;

            // Save data to a file for this time step
            std::string fileName = "Data/particles_" + std::to_string(t) + ".dat";
            std::ofstream outfile(fileName, std::ios::binary);
            if (outfile.is_open()) {
                for (int p = 0; p < particlesSize; ++p) {
                    outfile.write(reinterpret_cast<const char*>(&particles[t][p]), sizeof(Particle));
                }
                outfile.close();
            }
            else {
                std::cerr << "Error opening file for writing: " << fileName << std::endl;
            }

            std::cout << "Progress: " << (t * 100) / numTimeSteps << "%" << std::endl;
        }
    }
    std::cout << "Total Calculations: " << calulations << std::endl;
    return true;
}

void Physics::setRandomSeed(unsigned int seed)
{
	// Setze den Zufallszahlengenerator auf einen bestimmten Wert
    srand(seed);
}

double Physics::random(double min, double max)
{
    // Generiere eine zufällige Gleitkommazahl zwischen min und max
    double randomFloat = min + static_cast<double>(rand()) / static_cast<double>(RAND_MAX) * (max - min);

    return randomFloat;
}