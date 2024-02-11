#include "MathFunctions.h"
#include <random>
#include <chrono>
#include <cmath>
#include <cstdlib>


///////////////SPH Kernel Functions///////////////////////////////////////////

double MathFunctions::cubicSplineKernel(double r, double h)
{
    const double pi = 3.14159265359;
    const double alpha_3d = 1.0 / (pi * h * h * h);
    double q = r / h;
    if (q < 1.0) {
        return alpha_3d * (1 - 1.5 * q * q + 0.75 * q * q * q);
    }
    else if (q < 2.0) {
        return alpha_3d * 0.25 * pow(2 - q, 3);
    }
    return 0.0;
}

double MathFunctions::laplaceCubicSplineKernel(const glm::dvec3& rVec, double h)
{
    double r = glm::length(rVec);
    if (r > 2 * h) {
        return 0;
    }

    double sigma = 45.0 / (glm::pi<double>() * std::pow(h, 6));
    double factor;

    if (r <= h) {
        factor = sigma * (h - r) * (3.0 * h - 3.0 * r);
    }
    else {
        factor = sigma * 3.0 * std::pow(h - r, 2);
    }

    return factor;
}

//////////// Random Functions /////////////////////////////////////////////


double MathFunctions::gaussianRandom(double mean, double stddev)
{
    // Erstellen eines Zufallszahlengenerators
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);

    // Erstellen einer Normalverteilung
    std::normal_distribution<double> distribution(mean, stddev);

    // Generieren und Rückgabe einer Zahl aus dieser Verteilung
    return distribution(generator);

}

void MathFunctions::setRandomSeed(unsigned int seed)
{
    // Setze den Zufallszahlengenerator auf einen bestimmten Wert
    srand(seed);
}

double MathFunctions::random(double min, double max)
{
    // Generiere eine zufällige Gleitkommazahl zwischen min und max
    double randomFloat = min + static_cast<double>(rand()) / static_cast<double>(RAND_MAX) * (max - min);

    return randomFloat;
}