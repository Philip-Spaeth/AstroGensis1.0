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
    double lengthInitial = 1e21;
    double massInitial = 1e40;
    double timeInitial = 1e14;

//All in SI Units
    int numTimeSteps = 100;
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

    //SPH takes extra calculation time
    bool SPH = true;
    double h = 1e19;
    double k = 1e42;
    double rh0 = 0.5e-21;
    double mu = 1e47;
    double thermalConstant = 1e-3;
    //ok good for S0:      double h = 1e18;     double k = 3e46;    double rh0 = 1e-19;     double mu = 1e47;

    //SPH better functions (Extra Calculation Time
    bool normilizedPressureForce = true; // takes the direction of the other particles into account
    bool simplifiedDensity = false; // simplyfied density calculation using the Octree
    bool adaptiveSmoothingLength = false; // Takes extra calculation time
    double hFactor = 1e1;
    bool springelViscosity = false; // Springel more accurate and complicated
    double alpha = 2e9;
    double beta = 1e18;
    double gamma = 5 / 3;


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
    void calculateGravitation(int timeStap, int start, int stop);
};

#endif