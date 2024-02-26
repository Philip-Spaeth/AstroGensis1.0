#pragma once
#include <glm.hpp>
#include "gtc/matrix_transform.hpp"

class Particle 
{
public:

    Particle(double x = 0.0f, double y = 0.0f, double z = 0.0f);
    Particle(glm::dvec3 position);
    Particle(glm::dvec3 position, double mass);
    Particle(glm::dvec3 position, double mass, glm::dvec3 velocity);


    //Physically variables
    glm::dvec3 position = { 0,0,0 };
    glm::dvec3 velocity = { 0,0,0 };
    glm::dvec3 force = { 0,0,0 };
    double mass;
    //SPH variables
    double density = 0;
    double pressure = 0;
    double h = 0;
    double darkMatterDensity = 0;
    double baryonicDensity = 0;
    double thermalEnergyChange = 0;
    double thermalEnergy = 0;

    // RungeKuta variables
    glm::dvec3 YnVelocity = { 0,0,0 };
    glm::dvec3 k1Velocity = { 0,0,0 };
    glm::dvec3 k2Velocity = { 0,0,0 };
    glm::dvec3 k3Velocity = { 0,0,0 };
    glm::dvec3 k4Velocity = { 0,0,0 };
    glm::dvec3 k1Position = { 0,0,0 };
    glm::dvec3 k2Position = { 0,0,0 };
    glm::dvec3 k3Position = { 0,0,0 };
    glm::dvec3 k4Position = { 0,0,0 };


    //dark matter or not
    bool darkMatter = false;

    //dark Energy / hubbleconstant
    void hubbleExpansion(double deltaTime);


    //variables for render
    double radius;
    //visible color
    glm::vec3 color;
    //infrared
    glm::vec3 densityColor;
    glm::vec3 thermalColor = {1,1,1};
    //visible color

    void setColor(double mediumDensity, double mediumThermal);


    //SystemInit
    double angle = 0;

    //Help functions
    double CalculateDistance(const Particle& other) const;

    //Physically functions
    glm::dvec3 calculateGravitationalForce(const Particle& other, double G, double softening, double deltaTime);
    glm::dvec3 calculateGravitationalForce(const Particle& other, double G, double softening, double deltaTime, int k);
    glm::dvec3 calcAcceleration(glm::dvec3 force);


    //Diffrent numerical methods
    //semi-implicit Euler
    void eulerUpdateVelocity(glm::dvec3 acceleration, double deltaTime);
    void eulerUpdatePosition(glm::dvec3 velocity, double deltaTime);
    void eulerUpdateThermalEnergy(double deltaTime);

    //Runge-Kutta order 4
    void rungeKuttaUpdateVelocity(glm::dvec3 acceleration, double deltaTime, int rungeKutaStep);
	void rungeKuttaUpdatePosition(double deltaTime, int rungeKutaStep);

    //leapfrog
    void leapfrogUpdateVelocity(glm::dvec3 acceleration, double deltaTime);
    void leapfrogUpdatePosition(glm::dvec3 velocity, double deltaTime);



    //Energie functions
    double calcKineticEnergie();
    double calcPotentialEnergie(const Particle& other, double G, double softening);
    double calcPotentialEnergie(const Particle& other, double G, double softening, int k);
    double calcThermalEnergy();
};