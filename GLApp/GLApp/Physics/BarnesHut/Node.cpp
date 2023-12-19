#include "Node.h"
#include <iostream> 
#include "Physics.h"
#include <cmath>


// Smoothing Length (adjust based on galactic scales)
const double smoothingLength = 500.0;

// Rest Density
const double restDensity = 0.3;

// Stiffness (Gas Constant)
const double stiffness = 5.0;

// Constants for SPH kernels (adjust as needed)
const double PI = 3.14159265358979323846;
const double poly6Coefficient = 315.0 / (64.0 * PI * std::pow(smoothingLength, 9));
const double spikyGradientCoefficient = -45.0 / (PI * std::pow(smoothingLength, 6));
const double viscosityLaplacianCoefficient = 45.0 / (PI * std::pow(smoothingLength, 6));

// SPH kernel functions

// Poly6 kernel for density estimation
double kernelPoly6(double r) 
{
	if (r >= 0 && r <= smoothingLength) {
		double q = 1.0 - std::pow(r / smoothingLength, 2);
		return poly6Coefficient * std::pow(q, 3);
	}
	else {
		// Verwenden Sie hier eine andere Funktion, die für große Entfernungen geeignet ist.
		// Zum Beispiel eine konstante Dichte oder eine abgeschwächte Dichte.
		// Experimentieren Sie mit verschiedenen Ansätzen je nach Ihren Simulationserfordernissen.
		return 1.0; // Beispiel: Konstante Dichte für große Entfernungen
	}
}

// Spiky gradient kernel for pressure forces
glm::dvec3 kernelSpikyGradient(const glm::dvec3& delta, double r) {
	double distance = glm::length(delta);
	if (distance > 0 && distance <= smoothingLength) {
		double coefficient = spikyGradientCoefficient * std::pow(smoothingLength - distance, 2) / distance;
		return coefficient * delta;
	}
	else {
		return glm::dvec3(1.0);
	}
}

// Viscosity Laplacian kernel for viscosity forces
glm::dvec3 kernelViscosityLaplacian(const glm::dvec3& delta, double r) {
	double distance = glm::length(delta);
	if (distance > 0 && distance <= smoothingLength) {
		double coefficient = viscosityLaplacianCoefficient * (smoothingLength - distance);
		return coefficient * delta / distance;
	}
	else {
		return glm::dvec3(1.0);
	}
}

Node::Node(glm::dvec3 center, double radius, double theta, int index, int maxdepth, bool renderTree)
{
	this->center = center;
	this->radius = radius;
	this->theta = theta;
	this->index = index;
	this->maxDepth = maxdepth;
	this->renderTree = renderTree;
}

Node::~Node()
{
	for (Node* child : child)
	{
		if (child != nullptr)
		{
			delete child;
		}
	}
}

void Node::gravity(Particle& p, glm::dvec3& force, double softening, double a, double& potentialEngergy, double& calculations)
{
	double G = 6.67408e-11;

	if (particle.position != p.position)
	{
		glm::dvec3 delta = massCenter - p.position;

		double r = glm::length(delta);

		if (r > 0)
		{
			if (Physics::PlummerSoftening)
			{
				//Plummer softening
				double epsilon0 = softening / std::sqrt(1.0 + std::pow(r / a, 2));

				double distance = r * r + epsilon0 * epsilon0;

				//normal direct force
				double forceMagnitude = (G * mass * p.mass) / std::pow(distance, 3 / 2);
				glm::dvec3 Force = forceMagnitude * glm::normalize(delta);
				Particle p2 = Particle(massCenter, mass);
				potentialEngergy += p.calcPotentialEnergie(p2, G, epsilon0, 0);
				force += Force;
				calculations++;
			}
			else
			{
				double distance = r * r + softening * softening;
				//normal direct force
				double forceMagnitude = (G * mass * p.mass) / std::pow(distance, 3 / 2);
				glm::dvec3 Force = forceMagnitude * glm::normalize(delta);
				Particle p2 = Particle(massCenter, mass);
				potentialEngergy += p.calcPotentialEnergie(p2, G, softening, 0);
				force += Force;
				calculations++;
			}
		}
	}
}

void Node::gravitySPH(Particle& p, glm::dvec3& force, double softening, double a, double& potentialEngergy, double& calculations)
{
	glm::dvec3 delta = massCenter - p.position;
	double r = glm::length(delta);
	if (r > 0)
	{
		double G = 6.67408e-11;

		// SPH density estimation
		double density = kernelPoly6(r);
		p.density = density * mass;

		// SPH pressure calculation
		double pressure = stiffness * (p.density - restDensity);
		p.pressure = pressure * mass;

		// SPH forces
		glm::dvec3 pressureForce = (pressure + p.pressure) / (2 * p.density) *
			p.mass * kernelSpikyGradient(delta, smoothingLength);

		glm::dvec3 viscosityForce = (p.velocity - particle.velocity) / p.density *
			p.mass * kernelViscosityLaplacian(delta, smoothingLength);

		// Gravitational force
		double distance = r * r + softening * softening;
		//normal direct force
		double forceMagnitude = (G * mass * p.mass) / std::pow(distance, 3 / 2);
		glm::dvec3 gravityForce = forceMagnitude * glm::normalize(delta);

		// Total force
		glm::dvec3 totalForce = pressureForce + viscosityForce + gravityForce;

		// Update the force on the particle
		force += totalForce;
	}
}

glm::dvec3 Node::calcForce(Particle& p, double softening,double a, double& potentialEngergy, double& calculations)
{
	glm::dvec3 force = {0,0,0};

	if (isLeaf) 
	{
		if (Physics::SPH)
		{
			//SPH
			gravitySPH(p, force, softening, a, potentialEngergy, calculations);
		}
		else
		{
			//normal direct force
			gravity(p, force, softening, a, potentialEngergy, calculations);
		}
	}
	else 
	{
		glm::dvec3 delta = massCenter - p.position;
		double r = glm::length(delta);

		// Calculate height of the node
		double d = radius * 2; 

		if (d / r < theta)
		{
			if (Physics::SPH)
			{
				//SPH
				gravitySPH(p, force, softening, a, potentialEngergy, calculations);
			}
			else
			{
				//normal direct force
				gravity(p, force, softening, a, potentialEngergy, calculations);
			}
		}
		else
		{
			// For all child nodes n
			for (Node* child : child) 
			{
				if(child != nullptr)
				{
					force += child->calcForce(p, softening, a, potentialEngergy, calculations);
				}
			}
		}
	}
	return force;
}

void Node::insert(Particle& p)
{
	if (index < maxDepth)
	{
		if (mass == 0)
		{
			mass = p.mass;
			particle = p;
			isLeaf = true;
			massCenter = p.position;
		}
		else
		{
			isLeaf = false;
			//check in wich quadrant the particle is
			int quadrant = 0;
			if (p.position.x > center.x)
			{
				quadrant += 1;
			}
			if (p.position.y > center.y)
			{
				quadrant += 2;
			}
			if (p.position.z > center.z)
			{
				quadrant += 4;
			}

			if (child[quadrant] == nullptr)
			{
				//create new node
				glm::dvec3 newCenter = center;
				double newRadius = radius / 2;
				switch (quadrant)
				{
				case 0:
					newCenter.x -= newRadius;
					newCenter.y -= newRadius;
					newCenter.z -= newRadius;
					break;
				case 1:
					newCenter.x += newRadius;
					newCenter.y -= newRadius;
					newCenter.z -= newRadius;
					break;
				case 2:
					newCenter.x -= newRadius;
					newCenter.y += newRadius;
					newCenter.z -= newRadius;
					break;
				case 3:
					newCenter.x += newRadius;
					newCenter.y += newRadius;
					newCenter.z -= newRadius;
					break;
				case 4:
					newCenter.x -= newRadius;
					newCenter.y -= newRadius;
					newCenter.z += newRadius;
					break;
				case 5:
					newCenter.x += newRadius;
					newCenter.y -= newRadius;
					newCenter.z += newRadius;
					break;
				case 6:
					newCenter.x -= newRadius;
					newCenter.y += newRadius;
					newCenter.z += newRadius;
					break;
				case 7:
					newCenter.x += newRadius;
					newCenter.y += newRadius;
					newCenter.z += newRadius;
					break;
				}
				child[quadrant] = new Node(newCenter, newRadius, theta, index + 1, maxDepth, renderTree);
			}
			child[quadrant]->insert(p);

			mass += p.mass;
			massCenter = (massCenter * (mass - p.mass) + p.position * p.mass) / mass;

			if (particlePushed == false) {
				particlePushed = true;

				this->insert(particle);
			}


		}
	}
	else
	{
		mass += p.mass;
		massCenter = (massCenter * (mass - p.mass) + p.position * p.mass) / mass;
		//std::cout << "max depth reached in index: " << index << std::endl;
	}
	//std::cout << "inserted particle " << "in node " << index <<"  mass: " << mass << "radius: "<< radius << std::endl;
}

void Node::calcMass() 
{
	if (isLeaf)
	{
		massCenter = particle.position;
	}
	else
	{
		glm::dvec3 massCenterSum = { 0,0,0 };
		double massSum = 0;
		for (Node* child : child)
		{
			if (child != nullptr)
			{
				child->calcMass();
				massCenterSum += child->massCenter * child->mass;
				massSum += child->mass;
			}
		}
		massCenter = massCenterSum / massSum;
		mass = massSum;
	}
}

void Node::clear() {
	for(int i = 0; i < 8; i++)
	{
		if (child[i] != nullptr)
		{
			child[i]->clear();
			delete child[i];
			child[i] = nullptr;
		}
	}
}

