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

    bool Calc();

    void setRandomSeed(unsigned int seed);
    double random(double min, double max);

    static const int numTimeSteps = 10000;
    static const int particlesSize = 10;

    // calculation Method:
    int calculationMethod = 3;
    
    // one hour = 3600
    // one day = 86400
    // one year = 31536000
    // big galaxy = 1e16
    // galaxy = 1e13

    //the time per frame

    const double deltaTime = 100;


    const double theta = 0;
    const double maxDepth = 200;

    //Physikalische Konstanten
    const double G = 6.67430e-11;
    // softening factor
    //  galaxy = 1e18
    // sun system = 0.2
    const double softening = 1;
    double maxDistance = 0;

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

#endif // PHYSICS_H