#ifndef PHYSICS_H
#define PHYSICS_H

#include <vector>
#include <string>
#include <filesystem>
#include <chrono>
#include <thread>
#include "Particle.h"
#include <glm.hpp>
#include "BarnesHut/Octree.h"
#include "FileManager.h"
#include "Units.h"
#include <atomic>

#define TARGET_FPS 30

class SystemInit;
class FileManager;
class Octree;

class Physics {
public:
    Physics(std::string dataFolder = "Data");
    std::string dataFolder = "Data";

    bool configFile = true;

//Choose the Unitscaling for the simulation
    Units* units;
    //parsec
    double lengthInitial = 1;
    //solar mass
    double massInitial = 1;
    //time Unit is calculated so that the gravitational constant is 1

//All in SI Units
    int numTimeSteps = 10;
    int particlesSize = 20000;

    //the time per frame
    //Optimal Value for ellipticalgalaxy : 1e12
    double deltaTime = 1e14;
    // calculation Method:
    int calculationMethod = 1;

    //Softening
    bool PlummerSoftening = true;
    double softening = 1e18;
    double a = 1e100;

    //barnes hut
    double theta = 1;
    const double maxDepth = 100;
    const bool newDistanceCalc = false;
    double maxDistance = 0;

// SPH Parameters
    bool SPH = true;

    //dark Energy / Cosmological Constant
    bool darkEnergy = true;
    int HubbleConstant = 70;

    //Color of the Particles (only for OpenGL) takes extra calculation time
    static const bool color = true;
    double colorH = 1e19;

    //Physikalische Konstanten, unit of G is m^3 kg^-1 s^-2
    const double G = 6.67430e-11;

    //Date for short calculation
    bool showDate = false; // does not word on larger time scales like galaxys
    int year = 2020;
    int month = 1;
    int day = 1;

    bool init();
    bool Calc();

    std::vector<Particle> currentParticles;

    void config();
    void scaleUnits();
private:
    Octree* octree;
    std::vector<std::vector<double>> totalEnergie;
    SystemInit* systemInit;
    FileManager* fileManager;
    void calcTime(int index, std::chrono::system_clock::time_point current_time);
    std::chrono::system_clock::time_point time;
    void calculateGravitation(int timeStap);
    void calculateGravitation(int timeStap, int start, int end);
    std::atomic<int> taskIndex{0};
};

#endif