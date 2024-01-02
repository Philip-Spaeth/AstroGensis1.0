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
    Physics();

    static const int numTimeSteps = 1000;
    static const int particlesSize = 10000;

    //the time per frame
    const double deltaTime = 1e13;

    // calculation Method:
    int calculationMethod = 1;

    //Softening
    static const bool PlummerSoftening = true;
    const double softening = 1e18;
    const double a = 1e100;

    //barnes hut
    const double theta = 1;
    const double maxDepth = 30;

    //SPH
    static const bool SPH = true;
    const double h = 1e19;
    //const double k = 1e46;
    const double k = 1;
    const double rh0 = 1e-20;

    //dark Energy / Cosmological Constant
    static const bool darkEnergy = false;
    static const int HubbleConstant = 70;

    //Color of the Particles (only for OpenGL)
    static const bool color = true;

    //for the size of the octree
    double maxDistance = 0;

    //Physikalische Konstanten
    const double G = 6.67430e-11;

    bool Calc();

    void setRandomSeed(unsigned int seed);
    double random(double min, double max);

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