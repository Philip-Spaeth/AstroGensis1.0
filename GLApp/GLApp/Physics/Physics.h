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

#define TARGET_FPS 30

class SystemInit;
class FileManager;
class Octree;

class Physics {
public:
    Physics(std::string dataFolder = "Data");
    std::string dataFolder = "Data";

    bool configFile = true;

    int numTimeSteps = 2;
    int particlesSize = 20000;

    //the time per frame
    //Optimal Value for ellipticalgalaxy : 1e13
    double deltaTime = 1e14;
    // calculation Method:
    int calculationMethod = 1;

    //Softening
    bool PlummerSoftening = true;
    double softening = 1e18;
    double a = 1e100;

    //barnes hut
    double theta = 2.9;
    const double maxDepth = 100;
    const bool newDistanceCalc = false;
    double maxDistance = 0;

    //SPH takes extra calculation time
    bool SPH = true;
    double h = 1e19;
    double k = 1e46;
    double rh0 = 0.5e-21;
    double mu = 1e47;
    //ok good for S0:      double h = 1e18;     double k = 3e46;    double rh0 = 1e-19;     double mu = 1e47;

    //dark Energy / Cosmological Constant
    bool darkEnergy = true;
    int HubbleConstant = 70;

    //Color of the Particles (only for OpenGL) takes extra calculation time
    static const bool color = true;
    double colorH = 1e19;

    //Physikalische Konstanten
    const double G = 6.67430e-11;

    bool init();
    bool Calc();

    std::vector<Particle> currentParticles;
private:
    void config();
    Octree* octree;
    std::vector<std::vector<double>> totalEnergie;
    double calulations = 0;
    SystemInit* systemInit;
    FileManager* fileManager;
    void calcTime(int index, std::chrono::system_clock::time_point current_time);
    std::chrono::system_clock::time_point time;
    void calculateGravitation(int timeStap);
    void calculateGravitation(int timeStap, int start, int stop);
};

#endif