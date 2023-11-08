#include "Particle.h"

Particle::Particle(double x, double y, double z)  : position(x, y, z), velocity(0.0f, 0.0f, 0.0f), mass(0), radius(1.0f), color(glm::dvec3(1.0f, 1.0f, 1.0f))
{}


//Numerical methods

//Semi-Implicit Euler
void Particle::eulerUpdateVelocity(glm::dvec3 acceleration, double deltaTime)
{
	velocity += acceleration * deltaTime;
}

void Particle::eulerUpdatePosition(glm::dvec3 velocity, double deltaTime)
{
    position += velocity * deltaTime;
}

//Midpoint 2nd order
void Particle::midpointUpdateVelocity(glm::dvec3 acceleration, double deltaTime)
{
	glm::dvec3 velocityHalf = velocity + acceleration * deltaTime / 2.0;
	velocity += acceleration * deltaTime;
}
void Particle::midpointUpdatePosition(glm::dvec3 velocity, double deltaTime)
{
	position += velocity * deltaTime;
}

//Runge-Kutta 4th order
void Particle::rungeKuttaUpdateVelocity(glm::dvec3 acceleration, double deltaTime)
{
	glm::dvec3 k1 = acceleration * deltaTime;
	glm::dvec3 k2 = acceleration * deltaTime * 0.5;
	glm::dvec3 k3 = acceleration * deltaTime * 0.5;
	glm::dvec3 k4 = acceleration * deltaTime;

	velocity += (k1 + (double)2 * k2 + (double)2 * k3 + k4) / 6.0;
}
void Particle::rungeKuttaUpdatePosition(glm::dvec3 velocity, double deltaTime)
{
	position += velocity * deltaTime;
}

//Verlet
void Particle::verletUpdatePosition(glm::dvec3 acceleration, double deltaTime)
{
	position += velocity * deltaTime + acceleration * deltaTime * deltaTime / 2.0;
}
void Particle::verletUpdateVelocity(glm::dvec3 acceleration, glm::dvec3 accelerationOld, double deltaTime)
{
	velocity += (acceleration + accelerationOld) * deltaTime / 2.0;
}

//Leapfrog
void Particle::leapfrogUpdateVelocity(glm::dvec3 acceleration, double deltaTime)
{
	// Zuerst die Geschwindigkeit halbieren
	velocity += 0.5 * acceleration * deltaTime;
}

void Particle::leapfrogUpdatePosition(glm::dvec3 velocity, double deltaTime)
{
	// Dann die Position basierend auf der aktualisierten Geschwindigkeit aktualisieren
	position += velocity * deltaTime;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    


// Physically functions

glm::dvec3 Particle::calculateGravitationalForce(const Particle& other, double G, double softening, double deltaTime)
{
    glm::dvec3 delta = other.position - position;
    double distance = glm::length(delta);

    if (distance == 0)
    {
        return { 0, 0, 0 }; // Verhindere eine Division durch Null.
    }

    double forceMagnitude = (G * mass * other.mass) / (distance * distance + softening);
    glm::dvec3 force = forceMagnitude * glm::normalize(delta);

    return force;
}

glm::dvec3 Particle::calcAcceleration(glm::dvec3 force)
{
    //no division by zero
    if (mass == 0)
    {
		return { 0, 0, 0 };
	}

	glm::dvec3 acceleration = force / mass;
	return acceleration;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    

//Energie functions

double Particle::calcKineticEnergie()
{
	double kineticEnergie = 0.5 * mass * glm::length(velocity) * glm::length(velocity);
	return kineticEnergie;
}

double Particle::calcPotentialEnergie(const Particle& other, double G, double softening)
{
	glm::dvec3 delta = other.position - position;
	double distance = glm::length(delta);

    if (distance == 0)
    {
		return 0; // Verhindere eine Division durch Null.
	}

	double potentialEnergie = -G * mass * other.mass / (distance + softening);
	return potentialEnergie;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//help functions

double Particle::CalculateDistance(const Particle& other) const {
    glm::dvec3 delta = other.position - position;
    return glm::length(delta);
}