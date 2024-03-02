#include "Particle.h"
#include "Physics.h"
Particle::Particle(double x, double y, double z)  : position(x, y, z), velocity(0.0f, 0.0f, 0.0f), mass(0), radius(1.0f), color(glm::dvec3(1.0f, 1.0f, 1.0f))
{}
 
Particle::Particle(glm::dvec3 position) : position(position), velocity(0.0f, 0.0f, 0.0f), mass(0), radius(1.0f), color(glm::dvec3(1.0f, 1.0f, 1.0f))
{}

Particle::Particle(glm::dvec3 position, double mass) : position(position), velocity(0.0f, 0.0f, 0.0f), mass(mass), radius(1.0f), color(glm::dvec3(1.0f, 1.0f, 1.0f))
{}

Particle::Particle(glm::dvec3 position, double mass, glm::dvec3 velocity) : position(position), velocity(velocity), mass(mass), radius(1.0f), color(glm::dvec3(1.0f, 1.0f, 1.0f))
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

void Particle::eulerUpdateThermalEnergy(double deltaTime)
{
	thermalEnergy += thermalEnergyChange * deltaTime;
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
    //plummer softening
    softening = 1;
    softening = softening * softening;

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
        delta = other.position + other.k2Position - position + k2Position;
	}
    else if (k == 3) {
        delta = other.position + other.k3Position - position + k3Position;
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

	double potentialEnergie = (0.5)*((- G * mass * other.mass) / (distance));
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

    double potentialEnergie = (0.5) * ((- G * mass * other.mass) / (distance));
    return potentialEnergie;
}

double Particle::calcThermalEnergy()
{
	return thermalEnergy;
}

//dark energy / hubblekosntant
void Particle::hubbleExpansion(double deltaTime)
{
    glm::dvec3 delta = position;
    double distance = glm::length(delta);

    if (distance == 0) 
    {
        return; // Verhindere eine Division durch Null.
    }
    Physics phy;
    // Hubble-Effekt
    double hubbleEffect = phy.HubbleConstant * 1e-3 / 3.0857e22; // Umrechnung von km/s/Mpc in SI-Einheiten
    glm::dvec3 hubbleVelocity = delta * hubbleEffect; // Relative Geschwindigkeit aufgrund der Expansion


    // Aktualisiere die Geschwindigkeit des Partikels
    position += hubbleVelocity * deltaTime;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//help functions

double Particle::CalculateDistance(const Particle& other) const {
    glm::dvec3 delta = other.position - position;
    return glm::length(delta);
}

void Particle::setColor(double mediumDensity, double mediumThermal)
{
    //density color
    double maxDensity = mediumDensity;
    double midDensity = mediumDensity / 2;
    //set the color based on the density
    double r = density / midDensity;
    double g = 0;
    double b = (midDensity / 3) / density;
    
    double soft = 5;
    r = r + (density / (maxDensity * soft));
    g = g + (density / (maxDensity * soft));
    b = b + (density / (maxDensity * soft)) / 3;
    densityColor = { r, g, b };

    //thermal color
    //set the color based on the thermal energy
    /*
    r = 0;
    if (thermalEnergy > mediumThermal)
    {
		r = thermalEnergy / mediumThermal;
	}
    g = thermalEnergy * 5 / mediumThermal;
    b = (mediumThermal / 10) / thermalEnergy;
    */
    if (darkMatter)
    {
		r = 0;
		g = 0;
		b = 1;
	}
    else
    {
		r = 1;
		g = 0;
		b = 0;
	}
    thermalColor = { r, g, b};

    double brightness = density / mediumDensity;
    double yellowness = density * 50 / (mediumDensity * 100);
    r = 0.0 + yellowness + brightness;
    g = 0.0 + yellowness + brightness;
    b = 0.5 + brightness;
    // if one color is bigger than one scale all colors down so the biggest is 1
    if (r > 1 || g > 1 || b > 1) 
    {
		double max = r;
		if (g > max) {
			max = g;
		}
		if (b > max) {
			max = b;
		}
		r = r / max;
		g = g / max;
		b = b / max;
	}
    // if denisty is just a little bit bigger than the medium density make the color red
    if ((density * 0.4) > mediumDensity && (density * 0.3) < mediumDensity)
    {
        r = 1;
		g = 0;
		b = 0;
    }
    color = {r, g, b};
}