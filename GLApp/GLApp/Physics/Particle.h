#pragma once
#include <glm.hpp>
#include "gtc/matrix_transform.hpp"

class Particle 
{
public:

    Particle(double x = 0.0f, double y = 0.0f, double z = 0.0f);


    //Physically variables
    glm::dvec3 position;
    glm::dvec3 velocity;
    double mass;




    //variables for render
    bool colorMode = false;
    double radius;
    glm::vec3 color;
    double bigestGravitation = 0;

    //Help functions
    double CalculateDistance(const Particle& other) const;

    //Physically functions
    glm::dvec3 calculateGravitationalForce(const Particle& other, double G, double softening, double deltaTime);
    glm::dvec3 calcAcceleration(glm::dvec3 force);

    //Diffrent numerical methods
    void eulerUpdateVelocity(glm::dvec3 acceleration, double deltaTime);
    void eulerUpdatePosition(glm::dvec3 velocity, double deltaTime);

    //Energie functions
    void calcKineticEnergie();
};