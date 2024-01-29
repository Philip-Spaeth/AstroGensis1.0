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
    //Physics();
    Physics(std::string dataFolder = "Data");
    std::string dataFolder = "Data";

    static const int numTimeSteps = 1000;
    static const int particlesSize = 10;

    //the time per frame
    //Optimal Value for ellipticalgalaxy : 1e13
    const double deltaTime = 3600;

    // calculation Method:
    int calculationMethod = 1;

    //Softening
    static const bool PlummerSoftening = false;
    const double softening = 10;
    const double a = 1e100;

    //barnes hut
    const double theta = 1;
    const double maxDepth = 100;
    const bool newDistanceCalc = false;
    double maxDistance = 0;

    //SPH takes extra calculation time
    static const bool SPH = false;
    double h = 1e18;
    double k = 3e46;
    double rh0 = 1e-19;
    double mu = 1e47;
    //ok good for S0:      double h = 1e18;     double k = 3e46;    double rh0 = 1e-19;     double mu = 1e47;

    //dark Energy / Cosmological Constant
    static const bool darkEnergy = true;
    static const int HubbleConstant = 70;

    //Color of the Particles (only for OpenGL) takes extra calculation time
    static const bool color = true;
    double colorH = 1e19;

    //Physikalische Konstanten
    const double G = 6.67430e-11;

    bool Calc();

    void setRandomSeed(unsigned int seed);
    double random(double min, double max);
    double gaussianRandom(double mean = 0.0, double stddev = 1.0);

    std::vector<Particle> currentParticles;
private:
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