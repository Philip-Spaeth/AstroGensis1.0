#ifndef PHYSICS_H
#define PHYSICS_H

#include <vector>
#include <string>
#include <filesystem>
#include <chrono>
#include <thread>
#include "Particle.h"
#include <glm.hpp>

class SystemInit;

class Physics {
public:
    Physics();

    bool Calc(std::vector<std::vector<Particle>>& particles);

    void setRandomSeed(unsigned int seed);
    double random(double min, double max);

    const int numTimeSteps = 100;
    const int particlesSize = 9;

    const int batchSize = 10000;

    //Physikalische Konstanten
    const double G = 6.67430e-11;
    const double softening = 0.1;

    // one day = one second
    const double faktor = 1000;

private:
    int calulations = 0;
    float highestForce = 0;
    SystemInit* systemInit;
};

#endif // PHYSICS_H