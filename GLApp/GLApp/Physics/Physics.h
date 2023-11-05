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

#define TARGET_FPS 30

class SystemInit;

class Physics {
public:
    Physics();

    bool Calc(std::vector<std::vector<Particle>>& particles);

    void setRandomSeed(unsigned int seed);
    double random(double min, double max);

    const int numTimeSteps = 2000;
    const int particlesSize = 2;

    const int batchSize = 10000;

    //Physikalische Konstanten
    const double G = 6.67430e-11;
    const double softening = 0;


    // one hour per second = 3600
    // one day par second = 86400
    // one year per second = 31536000

    //set the simulated time per second
    const double faktor = 8640000 / TARGET_FPS;
    //const double faktor = 6e17 / TARGET_FPS;

private:
    int calulations = 0;
    double highestForce = 0;
    SystemInit* systemInit;
};

#endif // PHYSICS_H