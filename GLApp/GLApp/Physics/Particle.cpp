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

//Runge-Kutta 4th order
void Particle::rungeKuttaUpdateVelocity(glm::dvec3 acceleration, double deltaTime)
{
	//calculate the k1
	glm::dvec3 k1 = acceleration * deltaTime;
	//calculate the k2
	glm::dvec3 k2 = acceleration * (deltaTime / 2);
	//calculate the k3
	glm::dvec3 k3 = acceleration * (deltaTime / 2);
	//calculate the k4
	glm::dvec3 k4 = acceleration * deltaTime;

	velocity += (k1 + (double)2 * k2 + (double)2 * k3 + k4) / 6.0;
}

void Particle::rungeKuttaUpdatePosition(glm::dvec3 velocity, double deltaTime)
{
	position += velocity * deltaTime;
}


//Leapfrog
void Particle::leapfrogUpdateVelocity(glm::dvec3 acceleration, double deltaTime)
{
	//vn+1 = vn + an * dt/2
	velocity = velocity + acceleration * (deltaTime);
}

void Particle::leapfrogUpdatePosition(glm::dvec3 velocity, double deltaTime)
{
	//xn+1 = xn + vn+1/2 * dt/2
	position = position + velocity * (deltaTime);
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
	double kineticEnergie = 0.5 * mass * (glm::length(velocity) * glm::length(velocity));
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

	double potentialEnergie = (double)(1/2)*( - G * mass * other.mass / (distance));
	return potentialEnergie;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//help functions

double Particle::CalculateDistance(const Particle& other) const {
    glm::dvec3 delta = other.position - position;
    return glm::length(delta);
}