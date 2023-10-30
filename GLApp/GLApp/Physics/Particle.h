#pragma once
#include <glm.hpp>
#include "gtc/matrix_transform.hpp"

class Particle {
public:
    Particle(float x = 0.0f, float y = 0.0f, float z = 0.0f)
        : position(x, y, z), velocity(0.0f, 0.0f, 0.0f), mass(0
        ), radius(1.0f), color(glm::vec3(1.0f, 1.0f, 1.0f)) {
        // Weitere Initialisierungen, falls ben�tigt
    }

    glm::vec3 position ; // 3D-Position des Teilchens
    glm::vec3 velocity; // 3D-Geschwindigkeit des Teilchens
    float mass;         // Masse des Teilchens
    float radius;       // Radius des Teilchens
    glm::vec3 color;    //ds sds Farbe des Teilchens (RGB-Wert)

    bool colorMode = true;
    double bigestGravitation = 0;

    // Methode zur Aktualisierung der Position basierend auf der Geschwindigkeit
    void UpdatePosition(float deltaTime) 
    {
        position += velocity * deltaTime;
    }

    // Methode zur Berechnung der Entfernung zu einem anderen Teilchen
    float CalculateDistance(const Particle& other) const {
        glm::vec3 delta = other.position - position;
        return glm::length(delta);
    }

    glm::vec3 CalculateGravitationalForce(const Particle& other, double G, double softening, float deltaTime)
    {
        glm::vec3 zero = { 0,0,0 };

        if (position == zero && other.position == zero || mass == 0 || other.mass == 0)
        {
            return { 0 , 0 , 0 };
        }
        if (&other == this)
        {
            return { 0 , 0 , 0};
        } 

        glm::vec3 delta = other.position - position;
        float distance = glm::length(delta);

        float forceMagnitude = 0;

        if (distance != 0)
        {
            forceMagnitude = (G * mass * other.mass) / (distance * distance + softening * softening);
        }
        ////////////color mode
        if (colorMode) {
            if (other.mass > 100 && mass < 100) {
                if (forceMagnitude * -1 > bigestGravitation) {
                    static_cast<double>(bigestGravitation) = forceMagnitude * -1;
                }
                else if (forceMagnitude > bigestGravitation) {
                    static_cast<double>(bigestGravitation) = forceMagnitude;
                }
            }
        }

        float acceleration = 0;
        // Calculate the acceleration
        if (mass != 0)
        {
            acceleration = forceMagnitude / mass;
        }
        // Calculate the velocity in 3D
        glm::vec3 direction = glm::normalize(other.position - position);
        glm::vec3 newvelocity = (acceleration * deltaTime) * direction;

        // Update the velocity in 3D
        velocity += newvelocity;

        // Return the force vector
        return forceMagnitude * direction;
    }
};