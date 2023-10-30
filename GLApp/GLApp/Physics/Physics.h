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

    const int numTimeSteps = 3600;
    const int particlesSize = 10;

    const int batchSize = 10000;

    //Physikalische Konstanten
    const double G = 6.67430e-11;
    const double softening = 0.1;


    const double faktor = 100000;

private:
    int calulations = 0;
    float highestForce = 0;
    SystemInit* systemInit;
};

#endif // PHYSICS_H