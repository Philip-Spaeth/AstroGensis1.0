#define _USE_MATH_DEFINES
#include "Physics.h"
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <glm.hpp>
#include <gtc/constants.hpp>
#include "SystemInit.h"
#include <locale>
#include <string.h>
#include "FileManager.h"
#include <random>
#include <chrono>
#include <iostream>
#include <filesystem>
#include <string>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
namespace fs = std::filesystem;


Physics::Physics(std::string newDataFolder)
{
    dataFolder = newDataFolder;
    // Initialisieren des Zufallszahlengenerators
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
}

bool Physics::init()
{
    fileManager = new FileManager(dataFolder);
	std::cin.get();
    //Initilize the parameters based of the configfile
    if (configFile) config();

    // Select Folder save Data
    std::cout << "Please ENTER the name of the folder where the data should be saved.         Press ENTER to use default\n" << std::endl;
    std::string inputDataFolder;
    std::getline(std::cin, inputDataFolder);
    if (inputDataFolder != "") 
    {
        dataFolder = inputDataFolder;
    }
    else
    {
		dataFolder = "Data";
	}

    //create the folder
    std::string path = "Data/" + dataFolder;

    // Erstellen des Ordners
    // Überprüfen, ob der Ordner bereits existiert
    if (!std::filesystem::exists(path)) {
        // Erstellen des Ordners, wenn er nicht existiert
        if (!std::filesystem::create_directory(path)) {
            std::cerr << "Fehler beim Erstellen des Ordners: " << path << std::endl;
            return 1; // Beendet das Programm mit einem Fehlercode
        }
    }

    //create a new info.txt file where numerOfTimeSteps and and ParticlesSize is saved
    std::string infoFile = "Data/" + dataFolder + "/info.txt";
    std::ofstream file(infoFile);
    file << numTimeSteps << ";" << std::endl;
    file << particlesSize << ";" << std::endl;
    file << deltaTime << std::endl;
    file.close();

    // Select calculation method
    std::cout << "\nEnter the numerical method:        Press ENTER to use default" << std::endl;
    std::cout << "0 = Runge-Kutta 4th order" << std::endl;
    std::cout << "1 = Semi-Implicit Euler" << std::endl;
    std::cout << "2 = Drift-Kick-Drift Leapfrog" << std::endl;
    std::cout << "3 = Kick-Drift-Kick Leapfrog" << std::endl;
    std::string inputCalculationMethod;
    std::getline(std::cin, inputCalculationMethod);
    if (inputCalculationMethod != "") {
        calculationMethod = std::stoi(inputCalculationMethod);
    }


    std::cout << "\nStarting the calculations...\n" << std::endl;
    return true;
}

bool Physics::Calc()
{
    // Initialisieren des Systems
    if (!init())
    {
		return false;
    }

    auto current_time = std::chrono::system_clock::now();

    totalEnergie.resize(numTimeSteps);

    currentParticles.resize(particlesSize);

    for (int t = 0; t < numTimeSteps; ++t) 
    {
        totalEnergie[t].resize(particlesSize);  

        //Start values of the particles
        if (t == 0)
        {
            systemInit->start(currentParticles);

			for (int i = 0; i < currentParticles.size(); i++) 
            {
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
            
            double mediumDensity = 0;
            int densityN = 0;
            //only for color
            if (color)
            {
                for (int p = 0; p < currentParticles.size(); p++)
                {
                    currentParticles[p].density = 0;
                    //For color and SPH calculations
                    octree->calcdensity(currentParticles[p], colorH, mediumDensity, densityN);
                }
                //only for color
                mediumDensity = mediumDensity / densityN;
                for (int p = 0; p < currentParticles.size(); p++)
                {
                    if (color) currentParticles[p].setColor(mediumDensity);
                }
            }

            ///for SPH
            if (SPH)
            {
                for (int p = 0; p < currentParticles.size(); p++)
                {
                    currentParticles[p].density = 0;
                    //For color and SPH calculations
                    octree->calcdensity(currentParticles[p], h, mediumDensity, densityN);
                }
            }

            //rotation and masscurves
            fileManager = new FileManager(dataFolder);
            //fileManager->saveRotationCurve(currentParticles, "");
            //fileManager->saveMassCurve(currentParticles, "");
            fileManager->saveParticles(t, currentParticles, dataFolder);
        }


        else
        {
            if(newDistanceCalc)
            { 
                //mayby problem when particles fly far away
                for (int i = 0; i < currentParticles.size(); i++) {
                    double distance = sqrt(pow(currentParticles[i].position.x, 2) + pow(currentParticles[i].position.y, 2) + pow(currentParticles[i].position.z, 2));
                    if (distance > maxDistance) {
                        maxDistance = distance;
                    }
                }
                octree = new Octree(glm::dvec3(0, 0, 0), maxDistance * 2, theta, maxDepth);
            }

            // delte tree to clear memory
            octree->clearTree();
            //build a new tree
            octree->buildTree(currentParticles);

            double mediumDensity = 0;
            int densityN = 0;
            //only for color
            if (color)
            {
                for (int p = 0; p < currentParticles.size(); p++)
                {
                    currentParticles[p].density = 0;
                    //For color and SPH calculations
                    octree->calcdensity(currentParticles[p], colorH, mediumDensity, densityN);
                }
                //only for color
                mediumDensity = mediumDensity / densityN;
                for (int p = 0; p < currentParticles.size(); p++)
                {
                    if (color) currentParticles[p].setColor(mediumDensity);
                }
            }
           
            ///for SPH
            if (SPH)
            {
                for (int p = 0; p < currentParticles.size(); p++)
                {
                    currentParticles[p].density = 0;
                    //For color and SPH calculations
                    octree->calcdensity(currentParticles[p], h, mediumDensity, densityN);
                    if (adaptiveSmoothingLength)
                    {
                        currentParticles[p].h = pow((3 * currentParticles[p].mass) / (4 * glm::pi<double>() * currentParticles[p].density), 1.0 / 3.0) * hFactor;
                    }
                }
                //calc h for all nodes
                if (adaptiveSmoothingLength)
                {
                    octree->calcH();
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
                        //Euler Thermal Energy
                        currentParticles[p].eulerUpdateThermalEnergy(deltaTime);
                    }

                    //Drift-Kick-Drift leapfrog
                    if (calculationMethod == 2)
                    {
                        currentParticles[p].leapfrogUpdatePosition(currentParticles[p].velocity, deltaTime / 2);
                        currentParticles[p].leapfrogUpdateVelocity(currentParticles[p].calcAcceleration(currentParticles[p].force), deltaTime);
                        currentParticles[p].leapfrogUpdatePosition(currentParticles[p].velocity, deltaTime / 2);
                        //Euler Thermal Energy
                        currentParticles[p].eulerUpdateThermalEnergy(deltaTime);
                    }
                    //set the force to 0
                    currentParticles[p].force = { 0,0,0 };
                    currentParticles[p].thermalEnergyChange = 0;
                }
            }

            // Kick-Drift-Kick leapfrog
            if (calculationMethod == 3)
            {
                calculateGravitation(t);
                for (int p = 0; p < currentParticles.size(); p++) {
                    currentParticles[p].leapfrogUpdateVelocity(currentParticles[p].calcAcceleration(currentParticles[p].force), deltaTime / 2);
                    currentParticles[p].leapfrogUpdatePosition(currentParticles[p].velocity, deltaTime);

                    //set the force to 0
                    currentParticles[p].force = { 0,0,0 };
                    currentParticles[p].thermalEnergyChange = 0;
                }

                octree->clearTree();
                octree->buildTree(currentParticles);
                calculateGravitation(t);
                for(int p = 0; p < currentParticles.size(); p++)
				{
                    currentParticles[p].leapfrogUpdateVelocity(currentParticles[p].calcAcceleration(currentParticles[p].force), deltaTime / 2);
                    //Euler Thermal Energy
                    currentParticles[p].eulerUpdateThermalEnergy(deltaTime);

                    //set the force to 0
                    currentParticles[p].force = { 0,0,0 };
                    currentParticles[p].thermalEnergyChange = 0;
				}
            }

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
                            }
                        }
                        totalEnergie[t][p] += currentParticles[p].calcKineticEnergie();
                        currentParticles[p].thermalEnergyChange = 0;

                        //Runge-Kutta Schritte berechnen
                        currentParticles[p].rungeKuttaUpdateVelocity(currentParticles[p].calcAcceleration(totalForce), deltaTime, k);
                        currentParticles[p].rungeKuttaUpdatePosition(deltaTime, k);
                        //Euler Thermal Energy
                        currentParticles[p].eulerUpdateThermalEnergy(deltaTime);
                    }
                }
            }

            //Dark Energy / Hubbleconstant
            if (HubbleConstant)
            {
                for (int p = 0; p < currentParticles.size(); p++)
                {
                    currentParticles[p].hubbleExpansion(deltaTime);
                }
            }


            if(false)
            {
                //rotation and masscurves
                fileManager->saveMassCurve(currentParticles, "");
            }

            //std::cout <<currentParticles[485].thermalEnergy<< std::endl;

            fileManager->saveParticles(t, currentParticles, dataFolder);
            /*
            //andromeda vorsimulation
            double distanceBetween = 0;
            distanceBetween = sqrt(pow(currentParticles[0].position.x - currentParticles[1].position.x, 2) + pow(currentParticles[0].position.y - currentParticles[1].position.y, 2) + pow(currentParticles[0].position.z - currentParticles[1].position.z, 2));
            if (distanceBetween < 8e21)
            {
                double time = t * deltaTime;
                double VelocityRelativeToPartile0 = sqrt(pow(currentParticles[1].velocity.x - currentParticles[0].velocity.x, 2) + pow(currentParticles[1].velocity.y - currentParticles[0].velocity.y, 2) + pow(currentParticles[1].velocity.z - currentParticles[0].velocity.z, 2));
                std::cout << "currentTime: "<< time << "  Distance: " << distanceBetween <<"  Velocity: "<< VelocityRelativeToPartile0 << std::endl;
            }
            */
            calcTime(t, current_time);
        }
    }


    fileManager->saveEnergieData(totalEnergie, "../Energie_Daten/1000sec/Euler_Data.txt");

    std::cout << std::endl;
    return true;
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
        double l = 0;
        totalForce = octree->calculateForces(this, currentParticles[p], softening,a, potentialEngergy, l);
        totalEnergie[t][p] += potentialEngergy;
        totalEnergie[t][p] += currentParticles[p].thermalEnergy;
        currentParticles[p].force = totalForce;

        totalEnergie[t][p] += currentParticles[p].calcKineticEnergie();
    }
}


bool stringToBool(const std::string& str);

void Physics::config() {
    auto config = fileManager->readConfig("config.ini");

    // Zeitschritte
    if (config.find("Zeitschritte") != config.end()) {
        numTimeSteps = std::stoi(config["Zeitschritte"]);
    }
    else {
        std::cout << "Zeitschritte wurde nicht gefunden" << std::endl;
    }

    // DeltaT
    if (config.find("DeltaT") != config.end()) {
        deltaTime = std::stod(config["DeltaT"]);
    }
    else {
        std::cout << "DeltaT wurde nicht gefunden" << std::endl;
    }

    // Partikelanzahl
    if (config.find("Partikelanzahl") != config.end()) {
        particlesSize = std::stoi(config["Partikelanzahl"]);
    }
    else {
        std::cout << "Partikelanzahl wurde nicht gefunden" << std::endl;
    }

    // PlummerSoftening
    if (config.find("PlummerSoftening") != config.end()) {
        PlummerSoftening = config["PlummerSoftening"] == "true";
    }
    else {
        std::cout << "PlummerSoftening wurde nicht gefunden" << std::endl;
    }

    // SofteningLenght
    if (config.find("SofteningLenght") != config.end()) {
        softening = std::stod(config["SofteningLenght"]);
    }
    else {
        std::cout << "SofteningLenght wurde nicht gefunden" << std::endl;
    }

    // a
    if (config.find("a") != config.end()) {
        a = std::stod(config["a"]);
    }
    else {
        std::cout << "a wurde nicht gefunden" << std::endl;
    }

    // Theta
    if (config.find("Theta") != config.end()) {
        theta = std::stod(config["Theta"]);
    }
    else {
        std::cout << "Theta wurde nicht gefunden" << std::endl;
    }

    // SPH
    if (config.find("SPH") != config.end()) 
    {
        std::string l = config["SPH"];
        SPH = stringToBool(config["SPH"]);
    }
    else {
        std::cout << "SPH wurde nicht gefunden" << std::endl;
    }

    // h
    if (config.find("h") != config.end()) {
        h = std::stod(config["h"]);
    }
    else {
        std::cout << "h wurde nicht gefunden" << std::endl;
    }

    // k
    if (config.find("k") != config.end()) {
        k = std::stod(config["k"]);
    }
    else {
        std::cout << "k wurde nicht gefunden" << std::endl;
    }

    // rho0
    if (config.find("rho0") != config.end()) {
        rh0 = std::stod(config["rho0"]);
    }
    else {
        std::cout << "rho0 wurde nicht gefunden" << std::endl;
    }

    // mu
    if (config.find("mu") != config.end()) {
        mu = std::stod(config["mu"]);
    }
    else {
        std::cout << "mu wurde nicht gefunden" << std::endl;
    }
    //thermal const
    if (config.find("uConst") != config.end()) {
		thermalConstant = std::stod(config["uConst"]);
	}
    else {
		std::cout << "thermalConstant wurde nicht gefunden" << std::endl;
	}


    // HubbleConstant
    if (config.find("HubbleConstant") != config.end()) {
        HubbleConstant = std::stod(config["HubbleConstant"]);
    }
    else {
        std::cout << "HubbleConstant wurde nicht gefunden" << std::endl;
    }

    std::cout << std::endl;
}

bool stringToBool(const std::string& str) {
    // Trimmen des Strings und Entfernen von Kommentaren
    size_t commentPos = str.find_first_of("#;");
    std::string trimmedStr = (commentPos != std::string::npos) ? str.substr(0, commentPos) : str;
    trimmedStr.erase(0, trimmedStr.find_first_not_of(" \t"));
    trimmedStr.erase(trimmedStr.find_last_not_of(" \t") + 1);

    // Konvertierung in Kleinbuchstaben für vereinfachte Vergleiche
    std::string lowerStr = trimmedStr;
    std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);

    // Überprüfung auf bekannte Werte
    if (lowerStr == "1" || lowerStr == "true") {
        return true;
    }
    else if (lowerStr == "0" || lowerStr == "false") {
        return false;
    }
    else {
        throw std::invalid_argument("Invalid boolean string");
    }
}
