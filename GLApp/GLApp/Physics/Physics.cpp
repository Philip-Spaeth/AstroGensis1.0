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
    std::string input;
    std::getline(std::cin, input);

    while (true)
    {
        std::string dataPath = "Data"; // Pfad zum Data-Ordner

        std::cout << "Choose an old simulation or press ENTER to start a new one\n\n";
        std::cout << "Available simulations: \n\n";

        // Überprüfen, ob das Verzeichnis existiert und darauf zugegriffen werden kann
        if (fs::exists(dataPath) && fs::is_directory(dataPath)) {
            int i = 1;
            for (const auto& entry : fs::directory_iterator(dataPath)) {
                std::cout << "[" << i << "] " << entry.path().filename() << std::endl;
                i++;
            }
        }
        else {
            std::cout << "No available simulations. The 'Data' folder is missing or inaccessible.\n";
        }

        std::cout << "\nPress ENTER to start a new simulation\n\n";

        //check wich folder to use based on the i number from above
        std::string Input;
        std::getline(std::cin, Input);
        if (Input == "")
        {
            break;
        }
        else
        {
            int i = std::stoi(Input);
            int j = 1;
            for (const auto& entry : fs::directory_iterator("Data"))
            {
                if (i == j)
                {
                    dataFolder = entry.path().filename().string();

                    // Get the number of timesteps and the particlesize from the info.txt file, separated by ';'
                    std::string infoFile = "Data/" + dataFolder + "/info.txt";
                    std::ifstream file(infoFile);
                    if (!file.is_open()) {
                        std::cerr << "Error opening file: " << infoFile << std::endl;
                        return false;
                    }

                    std::string line;
                    if (std::getline(file, line)) {
                        try {
                            numTimeSteps = std::stoi(line);
                        }
                        catch (const std::invalid_argument& e) {
                            std::cerr << "Invalid number format for time steps: " << line << std::endl;
                            file.close();
                            return false;
                        }
                    }

                    if (std::getline(file, line)) {
                        try {
                            particlesSize = std::stoi(line);
                        }
                        catch (const std::invalid_argument& e) {
                            std::cerr << "Invalid number format for particle size: " << line << std::endl;
                            file.close();
                            return false;
                        }
                    }

                    file.close();
                    return false;
                }
                j++;
            }
            break;
        }
    }


    fileManager = new FileManager(dataFolder);
    //Initilize the parameters based of the configfile
    if (configFile) config();

    // Select calculation method
    std::cout << "Enter the calculation method:        Press ENTER to use default from Code" << std::endl;
    std::cout << "0 = Runge-Kutta 4th order" << std::endl;
    std::cout << "1 = Semi-Implicit Euler" << std::endl;
    std::cout << "2 = Drift-Kick-Drift Leapfrog" << std::endl;
    std::cout << "3 = Kick-Drift-Kick Leapfrog" << std::endl;
    std::string inputCalculationMethod;
    std::getline(std::cin, inputCalculationMethod);
    if (inputCalculationMethod != "") {
        calculationMethod = std::stoi(inputCalculationMethod);
    }

    // Select Folder save Data
    std::cout << "Bitte geben Sie einen Namen ein, unter welchen die Simulation gespeichert werden soll." << std::endl;
    std::cout << "ENTER standard: 'Data'." << std::endl;
    std::string inputDataFolder;
    std::getline(std::cin, inputDataFolder);
    if (inputDataFolder == "") inputDataFolder = "Data";
    dataFolder = inputDataFolder;
    //create a new info.txt file where numerOfTimeSteps and and ParticlesSize is saved
    std::string infoFile = "Data/" + dataFolder + "/info.txt";
    std::ofstream file(infoFile);
    file << numTimeSteps << ";" << std::endl;
    file << particlesSize << std::endl;
    file.close();

    std::cout << "Starting the calculations..." << std::endl;

    std::cout << "Folder: " << dataFolder << std::endl;
    std::string dataFile = "Data/" + dataFolder;

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
            if(false)
            {
                //rotation and masscurves
                fileManager->saveMassCurve(currentParticles, "");
            }

            fileManager->saveParticles(t, currentParticles, dataFolder);

            calcTime(t, current_time);
        }
    }


    fileManager->saveEnergieData(totalEnergie, "../Energie_Daten/1000sec/Euler_Data.txt");

    std::cout << std::endl;
    std::cout << "Total Calculations: " << calulations << std::endl;
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
        totalForce = octree->calculateForces(currentParticles[p], softening,a, potentialEngergy, calulations);
        totalEnergie[t][p] += potentialEngergy;
        currentParticles[p].force = totalForce;

        totalEnergie[t][p] += currentParticles[p].calcKineticEnergie();

        calulations++;
    }
}


bool stringToBool(const std::string& str);

void Physics::config() {
    auto config = fileManager->readConfig("config.ini");

    std::cout << "Using config File..." << std::endl;

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
    if (config.find("SPH") != config.end()) {
        SPH = config["SPH"] == "true";
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
    if (str == "1" || str == "true" || str == "True" || str == "TRUE") {
        return true;
    }
    else if (str == "0" || str == "false" || str == "False" || str == "FALSE") {
        return false;
    }
    else {
        throw std::invalid_argument("Invalid boolean string");
    }
}
