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

class SystemInit;

class Physics {
public:
    Physics();


    bool Calc(std::vector<std::vector<Particle>>& particles);

    void setRandomSeed(unsigned int seed);
    double random(double min, double max);

    const int numTimeSteps = 109;
    const int particlesSize = 10;

    const int batchSize = 10950;

    //Physikalische Konstanten
    const double G = 6.67430e-11;
    const double softening = 1e4;


    // one hour per second = 3600
    // one day par second = 86400
    // one year per second = 31536000
    // one century per second = 3153600000

    //set the simulated time per second
    const double faktor = 86400 / 30;

private:
    int calulations = 0;
    float highestForce = 0;
    SystemInit* systemInit;
};

#endif // PHYSICS_H