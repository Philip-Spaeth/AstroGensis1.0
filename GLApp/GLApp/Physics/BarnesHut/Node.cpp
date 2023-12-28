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
	if (index >= Physics::colorDepth)
	{
		//if the density is high make it more red and bright if it is low make it more blue and dark
		double mass = 0;
		if (isLeaf)
		{
			mass = particle.mass;
		}
		else
		{
			mass = this->mass;
		}
		Physics physics;
		double brightness = (mass - physics.minMass) / (physics.maxMass - physics.minMass);
		double red = brightness;
		double green = 0;
		double blue = 1 - brightness;
		double smoth = 0.9;
		//make the color more smooth
		red = red + smoth;
		green = 0;
		blue = blue + smoth;
		//set the color to all the particles in the childnodes
		for (Node* child : child)
		{
			if (child != nullptr)
			{
				child->color(glm::vec3(red,green,blue));
			}
		}
	}
	else
	{
		for (Node* child : child)
		{
			if (child != nullptr)
			{
				child->setColor();
			}
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


double Node::computeDensity(Particle& p, double h) {
	double density = 0.0;
	if (isLeaf) {
		double r = glm::length(p.position - particle.position);
		if (r < 2 * h) {
			density += particle.mass * cubicSplineKernel(r, h);
		}
	}
	else {
		for (Node* child : child) {
			if (child != nullptr) {
				density += child->computeDensity(p, h);
			}
		}
	}
	return density;
}

void Node::gravitySPH(Particle& p, Node* root, glm::dvec3& force, double softening, double a, double& potentialEngergy, double& calculations) 
{
	double h = 0.5e19; // Glättungsradius
	double k = 5e45; // Steifheitskonstante für Druck
	double rho0 = 2.4e-23; // Ruhe- oder Referenzdichte

	glm::dvec3 delta = massCenter - p.position;
	double r = glm::length(delta);
	double G = 6.67408e-11;

	if (p.position != particle.position && r > 0) {
		if (r < 2 * h) {
			// Dichte berechnen
			double density_i = root->computeDensity(p, h);
			double density_j = root->computeDensity(particle, h);

			// Druck berechnen
			double pressure_i = k * (density_i - rho0);
			double pressure_j = k * (density_j - rho0);

			// Gradient der Kernel-Funktion
			glm::dvec3 gradKernel = -glm::normalize(delta) * (cubicSplineKernel(r, h) / r);

			// Druckkraft berechnen
			glm::dvec3 pressureForce = -particle.mass * ((pressure_i / (density_i * density_i)) +
				(pressure_j / (density_j * density_j))) * gradKernel;

			// Kraft auf Partikel i anwenden
			force += pressureForce;
			//std::cout << "pressureForce: " << pressureForce.x << " " << pressureForce.y << " " << pressureForce.z << std::endl;
		}

		// Gravitationskraft
		glm::dvec3 gravityForce = (G * mass * p.mass) / (r * r + softening * softening) * glm::normalize(delta);
		force += gravityForce;
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
			p.halfsize = radius;
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
		p.halfsize = radius;
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

