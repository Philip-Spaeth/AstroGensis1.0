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
    std::cout << "Press enter to start the calculations or press o to use render the old calculations" << std::endl;

    std::string input;
    std::getline(std::cin, input);

    if (input == "o")
    {
        return false;
    }

    // Select calculation method
    std::cout << "Enter the calculation method:        Press ENTER to use default from Code" << std::endl;
    std::cout << "0 = Runge-Kutta 4th order" << std::endl;
    std::cout << "1 = Semi-Implicit Euler" << std::endl;
    std::cout << "2 = Drift-Kick-Drift Leapfrog" << std::endl;
    std::cout << "3 = Kick-Drift-Kick Leapfrog" << std::endl;
    std::string inputCalculationMethod;
    std::getline(std::cin, inputCalculationMethod);
    if(inputCalculationMethod != ""){
        calculationMethod = std::stoi(inputCalculationMethod);
    }

    auto current_time = std::chrono::system_clock::now();

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

			for (int i = 0; i < currentParticles.size(); i++) {
				double distance = sqrt(pow(currentParticles[i].position.x, 2) + pow(currentParticles[i].position.y, 2) + pow(currentParticles[i].position.z, 2));
				if (distance > maxDistance) {
					maxDistance = distance;
				}
			}
            octree = new Octree(glm::dvec3(0, 0, 0), maxDistance * 2, theta, maxDepth);
            // delte tree to clear memory
            octree->clearTree();
            //build a new tree
            octree->buildTree(currentParticles);
            //SPH density
            if (SPH)
            {
                for (int p = 0; p < currentParticles.size(); p++)
                {
                    currentParticles[p].density = 0;
                    octree->calcdensity(currentParticles[p], h);
                    //color based on density
                    if (color) currentParticles[p].setColor();

                }
            }

            //rotation and masscurves
            //fileManager->saveRotationCurve(currentParticles, "");
            //fileManager->saveMassCurve(currentParticles, "");

            fileManager = new FileManager();
            fileManager->saveParticles(t, currentParticles, "Data");
        }


        else
        {
            // delte tree to clear memory
            octree->clearTree();
            //build a new tree
            octree->buildTree(currentParticles);

            //SPH density
            if (SPH)
            {
                for (int p = 0; p < currentParticles.size(); p++)
                {
					currentParticles[p].density = 0;
					octree->calcdensity(currentParticles[p], h);
                    //color based on density
                    if (color) currentParticles[p].setColor();

				}
            }
            //Dark Energy / hubbleconstant
            if (HubbleConstant)
            {
                for (int p = 0; p < currentParticles.size(); p++)
                {
                    currentParticles[p].hubbleExpansion();
                }
            }

            //Other methods
            if (calculationMethod != 0 && calculationMethod != 3)
            {
                calculateGravitation(t);

                // particlesSize ist die Anzahl der Partikel
                for (int p = 0; p < currentParticles.size(); ++p)
                {

                    //semi implicit euler
                    if (calculationMethod == 1)
                    {
                        currentParticles[p].eulerUpdateVelocity(currentParticles[p].calcAcceleration(currentParticles[p].force), deltaTime);
                        currentParticles[p].eulerUpdatePosition(currentParticles[p].velocity, deltaTime);
                    }

                    //Drift-Kick-Drift leapfrog
                    if (calculationMethod == 2)
                    {
                        currentParticles[p].leapfrogUpdatePosition(currentParticles[p].velocity, deltaTime / 2);
                        currentParticles[p].leapfrogUpdateVelocity(currentParticles[p].calcAcceleration(currentParticles[p].force), deltaTime);
                        currentParticles[p].leapfrogUpdatePosition(currentParticles[p].velocity, deltaTime / 2);
                    }
                    //set the force to 0
                    currentParticles[p].force = { 0,0,0 };
                }
            }

            // Kick-Drift-Kick leapfrog
            if (calculationMethod == 3)
            {
                calculateGravitation(t);
                for (int p = 0; p < currentParticles.size(); p++) {
                    currentParticles[p].leapfrogUpdateVelocity(currentParticles[p].calcAcceleration(currentParticles[p].force), deltaTime / 2);
                    currentParticles[p].leapfrogUpdatePosition(currentParticles[p].velocity, deltaTime);
                }

                octree->clearTree();
                octree->buildTree(currentParticles);
                calculateGravitation(t);
                for(int p = 0; p < currentParticles.size(); p++)
				{
                    currentParticles[p].leapfrogUpdateVelocity(currentParticles[p].calcAcceleration(currentParticles[p].force), deltaTime / 2);
				}
            }

            // Aktualisiere den Octree basierend auf den neuen Partikelpositionen
            //Runge Kuta 
            if (calculationMethod == 0)
            {
                for (int k = 0; k < 4; k++) {
                    // Runge Kuta schritt fuer jeden Particle
                    for (int p = 0; p < particlesSize; ++p) {

                        // Das ergebniss dieser Berechnung gleibt bei jedem Zeitschritt gleich.
                        glm::dvec3 totalForce(0.0, 0.0, 0.0);
                        for (size_t j = 0; j < currentParticles.size(); j++)
                        {
                            if (p != j)
                            {
                                Particle& otherParticle = currentParticles[j];
                                glm::dvec3 force = currentParticles[p].calculateGravitationalForce(otherParticle, G, softening, deltaTime, k);
                                totalForce += force;
                                totalEnergie[t][p] += currentParticles[p].calcPotentialEnergie(otherParticle, G, 0, k);
                                calulations++;
                            }
                        }
                        totalEnergie[t][p] += currentParticles[p].calcKineticEnergie();

                        //Runge-Kutta Schritte berechnen
                        currentParticles[p].rungeKuttaUpdateVelocity(currentParticles[p].calcAcceleration(totalForce), deltaTime, k);
                        currentParticles[p].rungeKuttaUpdatePosition(deltaTime, k);
                    }
                }
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

void Physics::calcTime(int index, std::chrono::system_clock::time_point current_time)
{
    std::chrono::duration<double> elapsed_time = current_time - std::chrono::system_clock::now();

    double time = elapsed_time.count(); // Zeit in Sekunden
    std::string timeUnit = "Sekunden"; // oder eine andere Einheit Ihrer Wahl

    if (time < 0) {
        time *= -1; // Bei negativer Zeit den Betrag verwenden
    }

    //calculation of what to multiply with the time to get the remaining time so the it is 100
    // Berechnung der Zeitdifferenz in Sekunden
    /* std::chrono::duration<double> time_diff = time;

    // Sekunden in einen double-Wert umrechnen
    double time_seconds = time_diff.count();

    // Dann können Sie time_seconds mit index multiplizieren
    double newtime = time_seconds * (numTimeSteps - index) / index; */
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

void Physics::calculateGravitation(int t) {
    int num_threads = std::thread::hardware_concurrency();
    std::vector<std::thread> threads;

    int n = currentParticles.size(); // Gesamtanzahl der Iterationen
    int step = n / num_threads;

    for(int i = 0; i < num_threads; ++i) {
		threads.emplace_back([this, i, step, t]() {
			this->calculateGravitation(t, i * step, (i + 1) * step);
			});
	}

    // thrads abfangen
    for (auto& thread : threads)
    {
        thread.join();
    }
}

void Physics::calculateGravitation(int t, int start, int stop) {
    // particlesSize ist die Anzahl der Partikel
    for (int p = start; p < stop; ++p)
    {
        // Berechne die Gesamtkraft auf das Partikel
        glm::dvec3 totalForce = { 0,0,0 };
        double potentialEngergy = 0;
        totalForce = octree->calculateForces(currentParticles[p], softening,a, potentialEngergy, calulations);
        totalEnergie[t][p] += potentialEngergy;
        currentParticles[p].force = totalForce;

        totalEnergie[t][p] += currentParticles[p].calcKineticEnergie();

        calulations++;
    }
}