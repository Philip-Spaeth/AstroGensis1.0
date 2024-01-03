#include "Node.h"
#include <iostream> 
#include "Physics.h"
#include <cmath>

Node::Node(glm::dvec3 center, double radius, double theta, int index, int maxdepth, bool renderTree, glm::vec3* newparticleColor)
{
	this->center = center;
	this->radius = radius;
	this->theta = theta;
	this->index = index;
	this->maxDepth = maxdepth;
	this->renderTree = renderTree;
	particleColor = newparticleColor;
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
void Node::color(glm::vec3 color)
{
	if (isLeaf)
	{
		particleColor->r = color.r;
		particleColor->g = color.g;
		particleColor->b = color.b;
	}
	else
	{
		for (Node* child : child)
		{
			if (child != nullptr)
			{
				child->color(color);
			}
		}
	}
}

void Node::setColor()
{
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
				//double distance = r * r;
				//normal direct force
				double forceMagnitude = (G * mass * p.mass) / std::pow(distance, 1);
				glm::dvec3 Force = forceMagnitude * glm::normalize(delta);
				Particle p2 = Particle(massCenter, mass);
				potentialEngergy += p.calcPotentialEnergie(p2, G, softening, 0);
				force += Force;
				calculations++;
			}
		}
	}
}

double Node::cubicSplineKernel(double r, double h) 
{
	const double pi = 3.14159265359;
	const double alpha_3d = 1.0 / (pi * h * h * h);
	double q = r / h;
	if (q < 1.0) {
		return alpha_3d * (1 - 1.5 * q * q + 0.75 * q * q * q);
	}
	else if (q < 2.0) {
		return alpha_3d * 0.25 * pow(2 - q, 3);
	}
	return 0.0;
}

void Node::calcDensity(Particle& p, double h, double& medium, int& n)
{
	//choose the node in wich the particle is in and the radius of the node is the closest to h and than save the density of the node in the particle
	if (radius < h)
	{
		p.density = density;
		n++;
		medium += density;
	}
	else
	{
		// choose the child wich is the closest to the particle
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

		if (child[quadrant] != nullptr)
		{
			child[quadrant]->calcDensity(p, h, medium, n);
		}
		else
		{
			p.density = density;
			n++;
			medium += density;
		}
	}
}

glm::dvec3 Node::laplaceCubicSplineKernel(const glm::dvec3& rVec, double h) 
{
	double r = glm::length(rVec);
	if (r > 2 * h) {
		return glm::dvec3(0.0); // Außerhalb des Einflussbereichs
	}

	double sigma = 45.0 / (glm::pi<double>() * std::pow(h, 6));
	double factor;

	if (r <= h) {
		factor = sigma * (h - r) * (3.0 * h - 3.0 * r);
	}
	else {
		factor = sigma * 3.0 * std::pow(h - r, 2);
	}

	return factor * glm::normalize(rVec);
}

void Node::gravitySPH(Particle& p, Node* root, glm::dvec3& force, double softening, double a, double& potentialEngergy, double& calculations) 
{
	Physics physics;
	double h = physics.h; // Glättungsradius
	double k = physics.k; // Steifheitskonstante für Druck
	double rho0 = physics.rh0; // Ruhe- oder Referenzdichte

	glm::dvec3 delta = massCenter - p.position;
	double r = glm::length(delta);
	double G = 6.67408e-11;

	if (r > 0) 
	{
		// Gravitationskraft
		glm::dvec3 gravityForce = (G * mass * p.mass) / (r * r + softening * softening) * glm::normalize(delta);
		force += gravityForce;

		//Druckkraft
		{
			// Dichte berechnen
			double density_i = p.density;

			// Druck berechnen
			double pressure_i = k * (density_i - rho0);

			// Gradient der Kernel-Funktion
			glm::dvec3 gradKernel = glm::normalize(delta) * (cubicSplineKernel(r, h) / r);

			//(original)
			double pressureForce = - particle.mass * (pressure_i / (density_i * density_i));
			//nur wenn abstoßend
			if (pressureForce < 0)
			{
				glm::dvec3 vecPressureForce = pressureForce * gradKernel;
				// Kraft auf Partikel i anwenden
				force += vecPressureForce;
			}
		}

		//Viskosen Kraft
		//if (false)
		{
			double mu = physics.mu; // Setzen Sie einen geeigneten Wert
			glm::dvec3 velocityDiff = p.velocity - particle.velocity;
			double distance = glm::length(p.position - massCenter);
			glm::dvec3 laplaceKernelGrad = laplaceCubicSplineKernel(delta, h);

			glm::dvec3 viscousForce = -mu * (mass / p.density) * (velocityDiff / (distance + softening)) * laplaceKernelGrad;
			force += viscousForce;
		}
	}

	calculations++;
}


glm::dvec3 Node::calcForce(Particle& p, Node* root, double softening,double a, double& potentialEngergy, double& calculations)
{
	glm::dvec3 force = {0,0,0};

	if (isLeaf) 
	{
		if (Physics::SPH)
		{
			//SPH
			gravitySPH(p, root, force, softening, a, potentialEngergy, calculations);
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
				gravitySPH(p, root, force, softening, a, potentialEngergy, calculations);
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
					force += child->calcForce(p, root, softening, a, potentialEngergy, calculations);
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
			particleColor = &p.color;
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
				child[quadrant] = new Node(newCenter, newRadius, theta, index + 1, maxDepth, renderTree,new  glm::vec3(1, 0, 0));
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
	//calc the density of the node
	density = mass / (4.0 / 3.0 * 3.14159265359 * radius * radius * radius);

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

void Node::clear() 
{
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


bool Node::isInside(Particle& p){
	if (p.position.x > center.x + radius || p.position.x < center.x - radius)
	{
		return false;
	}
	if (p.position.y > center.y + radius || p.position.y < center.y - radius)
	{
		return false;
	}
	if (p.position.z > center.z + radius || p.position.z < center.z - radius)
	{
		return false;
	}
	return true;
}