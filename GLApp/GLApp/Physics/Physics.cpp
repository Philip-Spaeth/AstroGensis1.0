﻿#define _USE_MATH_DEFINES
#include "Physics.h"
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <cmath>
#include <glm.hpp>
#include <gtc/constants.hpp>
#include <fstream>
#include "SystemInit.h"
#include <locale>
#include <string.h>
#include "FileManager.h"

Physics::Physics()
{
    // Initialisieren des Zufallszahlengenerators
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
}

bool Physics::Calc()
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

    totalEnergie.resize(numTimeSteps);

    currentParticles.resize(particlesSize);

    for (int t = 0; t < numTimeSteps; ++t) 
    {
        totalEnergie[t].resize(particlesSize);

        //Start values of the particles
        if (t == 0)
        {
            systemInit = new SystemInit();
            systemInit->start(currentParticles);
            fileManager = new FileManager();
            fileManager->saveParticles(t, currentParticles, "Data");
        }

        else
        {

            for (int p = 0; p < particlesSize; ++p)
            {
                glm::dvec3 totalForce(0.0, 0.0, 0.0);

                //Drift-Kick-Drift leapfrog
                //currentParticles[p].leapfrogUpdatePosition(currentParticles[p].velocity, deltaTime / 2);

                for (size_t j = 0; j < currentParticles.size(); j++)
                {
                    if (p != j)
                    {
                        Particle& otherParticle = currentParticles[j];
                        glm::dvec3 force = currentParticles[p].calculateGravitationalForce(otherParticle, G, softening, deltaTime);
                        totalEnergie[t][p] += currentParticles[p].calcPotentialEnergie(otherParticle, G, softening);
                        totalForce += force;
                        calulations++;
                    }
                }

                totalEnergie[t][p] = currentParticles[p].calcKineticEnergie();

                //semi implicit euler
                currentParticles[p].eulerUpdateVelocity(currentParticles[p].calcAcceleration(totalForce), deltaTime);
                currentParticles[p].eulerUpdatePosition(currentParticles[p].velocity, deltaTime);
                
                //Runge-Kutta
                //currentParticles[p].rungeKuttaUpdateVelocity(currentParticles[p].calcAcceleration(totalForce), deltaTime);
				//currentParticles[p].rungeKuttaUpdatePosition(currentParticles[p].velocity, deltaTime);

                //Drift-Kick-Drift leapfrog
                //currentParticles[p].leapfrogUpdateVelocity(currentParticles[p].calcAcceleration(totalForce), deltaTime);
                //currentParticles[p].leapfrogUpdatePosition(currentParticles[p].velocity, deltaTime/2);
            }

            fileManager->saveParticles(t, currentParticles, "Data");

            calcTime(t, current_time);
        }
    }


    fileManager->saveEnergieData(totalEnergie, "../Energie_Daten/1000sec/Euler_Data.txt");

    std::cout << std::endl;
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

void Physics::calcTime(int index, std::chrono::steady_clock::time_point current_time)
{
    std::chrono::duration<double> elapsed_time = current_time - std::chrono::high_resolution_clock::now();

    double time = elapsed_time.count() * -1;
    std::string timeUnit;

    //calculation of what to multiply with the time to get the remaining time so the it is 100
    double newtime = time * (numTimeSteps - index) / index;


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
    std::cout << "Progress: " << (index * 100) / numTimeSteps << "%  " << (int)newtime << timeUnit << " left" << std::endl;
}