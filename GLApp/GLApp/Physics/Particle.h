#pragma once
#include <glm.hpp>

class Particle {
public:
    Particle(float x = 0.0f, float y = 0.0f, float z = 0.0f)
        : position(x, y, z), velocity(0.0f, 0.0f, 0.0f), mass(1.0f), radius(1.0f), color(glm::vec3(1.0f, 1.0f, 1.0f)) {
        // Weitere Initialisierungen, falls benötigt
    }

    glm::vec3 position; // 3D-Position des Teilchens
    glm::vec3 velocity; // 3D-Geschwindigkeit des Teilchens
    float mass;         // Masse des Teilchens
    float radius;       // Radius des Teilchens
    glm::vec3 color;    // Farbe des Teilchens (RGB-Wert)

    bool colorMode = false;
    double bigestGravitation = 0;

    // Methode zur Aktualisierung der Position basierend auf der Geschwindigkeit
    void UpdatePosition(float deltaTime) {
        position += velocity * deltaTime;
    }

    // Methode zur Berechnung der Entfernung zu einem anderen Teilchen
    float CalculateDistance(const Particle& other) const {
        glm::vec3 delta = other.position - position;
        return glm::length(delta);
    }

    // Methode zur Berechnung der Gravitationskraft von einem anderen Teilchen
    glm::vec3 CalculateGravitationalForce(const Particle& other, double G, double softening) const
    {
        glm::vec3 delta = other.position - position;
        float distance = glm::length(delta);
        if (distance < radius + other.radius) {
            // Kollision vermeiden (falls erforderlich)
            return glm::vec3(0.0f);
        }
        double forceMagnitude = (G * mass * other.mass) / (distance * distance + softening * softening);

        ////////////color mode
        if (colorMode)
        {
            if (other.mass > 100 && mass < 100)
            {
                if (forceMagnitude * -1 > bigestGravitation)
                {
                    static_cast<double>(bigestGravitation) = forceMagnitude * -1;
                }

                else if (forceMagnitude > bigestGravitation)
                {
                    static_cast<double>(bigestGravitation) = forceMagnitude;
                }
            }
		}

        return (delta / distance) * static_cast<float>(forceMagnitude);
    }

    // Methode zur Aktualisierung der Geschwindigkeit basierend auf einer Kraft
    void UpdateVelocity(const glm::vec3& force, float deltaTime) {
        velocity += (force / mass);
    }
};