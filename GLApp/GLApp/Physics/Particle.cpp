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
void Particle::rungeKuttaUpdateVelocity(glm::dvec3 acceleration, double deltaTime, int rungeKutaStep)
{
    YnVelocity = acceleration * deltaTime;
    if (rungeKutaStep == 0) 
    {
        k1Velocity = YnVelocity;
    }
    else if (rungeKutaStep == 1) 
    {
        k2Velocity = YnVelocity;
    }
    else if (rungeKutaStep == 2) 
    {
        k3Velocity = YnVelocity;
    }
    else if (rungeKutaStep == 3) 
    {
        k4Velocity = YnVelocity;

        // alle Ergebnisse Zusammenfassen:
        velocity += (k1Velocity + 2.0 * k2Velocity + 2.0 * k3Velocity + k4Velocity) * (1.0 / 6.0);
    }
}

void Particle::rungeKuttaUpdatePosition(double deltaTime, int rungeKutaStep)
{
    if (rungeKutaStep == 0) {
        k1Position = velocity * deltaTime;
    }
    else if (rungeKutaStep == 1) {
        k2Position = (velocity + k1Velocity * 0.5) * deltaTime;
    }
    else if (rungeKutaStep == 2) {
        k3Position = (velocity + k2Velocity * 0.5) * deltaTime;
    }
    else if (rungeKutaStep == 3) {
        k4Position = (velocity + k3Velocity * 0.5) * deltaTime;

        // alle Ergebnisse zusammenfassen:
        position += (k1Position + 2.0 * k2Position + 2.0 * k3Position + k4Position) * (1.0/6.0);
    }
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
glm::dvec3 Particle::calculateGravitationalForce(const Particle& other, double G, double softening, double deltaTime, int k)
{
   
    glm::dvec3 delta = other.position - position;
    if (k == 1) {
        // hier villeicht auch so: delta = other.position + other.k1Position  - position + k1Position;
        // und bei k2, k3 auch so
        delta = other.position + other.k1Position - position + k1Position;
	}
	else if (k == 2) {
        delta = other.position + other.k2Position - position + k1Position;
	}
    else if (k == 3) {
        delta = other.position + other.k3Position - position + k1Position;
    }
    else if (k == 4) {
        delta = other.position + other.k4Position - position + k1Position;
	}

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
double Particle::calcPotentialEnergie(const Particle& other, double G, double softening, int k)
{
    glm::dvec3 delta = other.position - position;
    if (k == 1) {
        delta = other.position + other.k1Position - position + k1Position;
    }
    else if (k == 2) {
        delta = other.position + other.k2Position - position + k2Position;
    }
    else if (k == 3) {
        delta = other.position + other.k3Position - position + k3Position;
    }
    double distance = glm::length(delta);

    if (distance == 0)
    {
        return 0; // Verhindere eine Division durch Null.
    }

    double potentialEnergie = (double)(1 / 2) * (-G * mass * other.mass / (distance));
    return potentialEnergie;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//help functions

double Particle::CalculateDistance(const Particle& other) const {
    glm::dvec3 delta = other.position - position;
    return glm::length(delta);
}