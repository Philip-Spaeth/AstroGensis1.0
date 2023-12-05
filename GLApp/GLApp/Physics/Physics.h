#ifndef PHYSICS_H
#define PHYSICS_H

#include <vector>
#include <string>
#include <filesystem>
#include <chrono>
#include <thread>
#include "Particle.h"
#include <glm.hpp>
#include "Engine.h"
#include "BarnesHut/Octree.h"
#include "FileManager.h"

#define TARGET_FPS 30

class SystemInit;
class FileManager;
class Octree;

class Physics {
public:
    Physics();

    bool Calc();

    void setRandomSeed(unsigned int seed);
    double random(double min, double max);

    static const int numTimeSteps = 10;
    static const int particlesSize = 10000;

    // calculation Method:
    int calculationMethod = 2;
    
    // one hour = 3600
    // one day = 86400
    // one year = 31536000
    // galaxy = 1e16
    // bigger galaxy = 3e15

    //the time per frame
    const double deltaTime = 3e15;

    const double theta = 4;
    const double maxDepth = 100;

    //Physikalische Konstanten
    const double G = 6.67430e-11;
    // softening factor
    //  galaxy = 1e18
    // sun system = 0.2
    const double softening = 1e10;
    double maxDistance = 0;

    std::vector<Particle> currentParticles;
private:
    Octree* octree;
    std::vector<std::vector<double>> totalEnergie;
    double calulations = 0;
    SystemInit* systemInit;
    FileManager* fileManager;
    void calcTime(int index, std::chrono::steady_clock::time_point current_time);
};

#endif // PHYSICS_H