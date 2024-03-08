#include <iostream>
#include <cmath>
#include <vector>
#include "Halo.h"
#include "MathFunctions.h"
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

// NFW Density Profile Function
double nfwProfile(double r, double rho0, double rs) {
	return rho0 / ((r / rs) * pow(1 + (r / rs), 2));
}

double nfwPotential(double r, double rho0, double rs) 
{
	double G = 6.67430e-11;
	return -4 * 3.14159265358979323846 * G * rho0 * (pow(rs,3) / r) * log( 1.0 + (r / rs));
}


void Halo::halo(int startIndex, int endIndex, glm::dvec3 position, glm::dvec3 rotation, glm::dvec3 velocity, double maxRadius, double Masse, std::vector<Particle>& particles)
{
    int size = endIndex - startIndex;
	//create halo
	for (int i = startIndex; i < endIndex; i++)
	{
        double c = 0e-5;
        double rvir = maxRadius;
        double rs = maxRadius;

		double G = 6.67430e-11;
        double pi = 3.14159265358979323846;
        // Parameters for NFW Profile
        double rho0 = Masse / (4.0 * pi * pow(rs, 3) * (log(1 + (rvir / rs)) - rvir / (rvir + rs)));

        // Calculate cumulative mass
        double totalMass = 0.0;
        for (int i = 1; i <= size; ++i) {
            double radius = rs * i / size;
            totalMass += 4.0 * pi * pow(radius, 2) * nfwProfile(radius, rho0, rs) * (rs / size);
        }

        double targetMass = i * totalMass / (size - 1);
        double radius = 0.0;
        double mass = 0.0;

        // Find the radius that corresponds to the target mass
        while (mass < targetMass) {
            radius += rs / size;
            mass += 4.0 * pi * pow(radius, 2) * nfwProfile(radius, rho0, rs) * (rs / size);
        }
		double r = radius;
		if (i == 0) r = 0;

		double potential = nfwPotential(r, rho0, rs);
        // random velocity
		double velocityMagnetude = sqrt(2.0 * abs(potential)) / 3;

        double cosTheta = 2.0 * (rand() / (double)RAND_MAX) - 1.0;
        double sinTheta = sqrt(1.0 - cosTheta * cosTheta);
        double phi = 2.0 * pi * (rand() / (double)RAND_MAX);
        glm::dvec3 randomPositionDir = glm::dvec3(sinTheta * cos(phi), sinTheta * sin(phi), cosTheta);

        double theta = acos(2 * MathFunctions::random(0, 1) - 1);
        double phi2 = 2 * pi * MathFunctions::random(0,1);

		particles[i].position = position + r * randomPositionDir;
        particles[i].velocity = velocity + glm::dvec3(velocityMagnetude * sin(theta) * cos(phi2), velocityMagnetude * sin(theta) * sin(phi2), velocityMagnetude * cos(theta));
        particles[i].mass = Masse / size;
		particles[i].radius = 1;
		particles[i].color = glm::vec3(1, 1, 1);
		particles[i].darkMatter = true;
	}
}