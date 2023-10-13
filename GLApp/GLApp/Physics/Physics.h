#ifndef PHYSICS_H
#define PHYSICS_H

#include <vector>
#include <string>
#include <filesystem>

#include "Particle.h"
#include <glm.hpp>

class SystemInit;

class Physics {
public:
    Physics();

    bool Calc(std::vector<std::vector<Particle>>& particles);

    void setRandomSeed(unsigned int seed);
    double random(double min, double max);

    const int numTimeSteps = 400;
    const int particlesSize = 1000;

    const int batchSize = 10000;

    //Physikalische Konstanten
    const double timepercalc = 0.5916079;
    const double G = 6.67430e-11;
    const double softening = 1e-1;


    const double faktor = 10000;

private:
    int calulations = 0;
    float highestForce = 0;
    SystemInit* systemInit;
};

#endif // PHYSICS_H