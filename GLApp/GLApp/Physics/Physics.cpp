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
#include <string.h>


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

    auto current_time = std::chrono::high_resolution_clock::now();

    std::cout << "Starting the calculations..." << std::endl;

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

                    if (i == p) 
                    {
                        continue;
                    }
                    if (particles[t][p].mass == 0 || particles[t][i].mass == 0)
                    {
						continue;
					}

                    glm::dvec3 force = currentParticle.CalculateGravitationalForce(particles[t][i], G, softening, faktor);

                    calulations++;
                }

                // Berechnen der neuen Position
                currentParticle.UpdatePosition(faktor);

                // calc Energie of system with each particle
                particles[t][p].calcKineticEnergie();

                //just for color not effecient
                if (currentParticle.colorMode == true)
                {
                    if (currentParticle.mass < 1000)
                    {
                        double gravityForce = particles[t][p].bigestGravitation;

                        if (gravityForce > highestForce)
                        {
                            if (particles[t][p].mass < 1000)
                            {
                                highestForce = gravityForce;
                            }
                        }

                        double color = gravityForce * 5 / highestForce;
                        double extraLight = 0.7;
                        particles[t][p].color = glm::dvec3(color/2 + extraLight, color/5 + extraLight, color + extraLight);
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

            std::chrono::duration<double> elapsed_time = current_time - std::chrono::high_resolution_clock::now();

            double time = elapsed_time.count() * -1;
            std::string timeUnit;

            //calculation of what to multiply with the time to get the remaining time so the it is 100
            double newtime = time * (numTimeSteps - t) / t;


            if (newtime <= 60)
            {
                timeUnit = " sec";
            }
            else if (newtime <= 3600)
            {
                timeUnit = " min";
                newtime /= 60;
            }
            else if (newtime <= 86400)
            {
                timeUnit = " h";
                newtime /= 3600;
            }
            else if (newtime > 86400)
            {
                timeUnit = " days";
                newtime /= 86400;
            }
            
            //printing out the progress
            std::cout << "Progress: " << (t * 100) / numTimeSteps << "%  "  << (int)newtime << timeUnit << " left" << std::endl;
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