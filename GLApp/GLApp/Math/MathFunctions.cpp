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
    double r = sqrt(rVec.x * rVec.x + rVec.y * rVec.y + rVec.z * rVec.z);
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

glm::dvec3 MathFunctions::gradientCubicSplineKernel(const glm::dvec3& r, double h) 
{
    double q = sqrt(r.x * r.x + r.y * r.y + r.z * r.z) / h;
    double sigma = 10.0 / (7.0 * 3.14 * pow(h, 2));
    glm::dvec3 gradW(0.0);

    if (q > 0 && q <= 1.0) {
        gradW = sigma * (-3 * q + 2.25 * q * q) * r / (glm::length(r) * h);
    }
    else if (q > 1.0 && q <= 2.0) {
        gradW = sigma * -0.75 * pow(2 - q, 2) * r / (glm::length(r) * h);
    }

    return gradW;
}

double MathFunctions::tempretureToInternalEnergy(double tempreture)
{
    double gamma = 5.0 / 3.0;
    double protonMassinKG = 1.6726219e-27;
    double boltzmannConstant = 1.38064852e-23;
    //helium
    double meanMolecularWeight = 0.000588235;

    return (tempreture * boltzmannConstant) / ((gamma - 1) * meanMolecularWeight * protonMassinKG);
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