#include "Node.h"
#include <iostream> 
#include "Physics.h"
#include <cmath>
#include "MathFunctions.h"

Node::Node(glm::dvec3 center, double radius, double theta, int index, int maxdepth, bool renderTree, glm::vec3 newparticleColor)
{
	this->center = center;
	this->radius = radius;
	this->theta = theta;
	this->index = index;
	this->maxDepth = maxdepth;
	this->renderTree = renderTree;
	particleColor = newparticleColor;
}

// just for first Node because first Node is not allowed to delete
void Node::clear(){
	for (int i = 0; i < 8; i++)
	{
		if(child[i] != nullptr){
			delete child[i];
			child[i] = nullptr; // Verhindert Zugriff auf freigegebenen Speicher
		}
	}
}

Node::~Node()
{
	for (int i = 0; i < 8; i++)
	{
		if(child[i] != nullptr){
			//particleColor = nullptr;
			delete child[i];
			child[i] = nullptr; // Verhindert Zugriff auf freigegebenen Speicher
		}
	}
	//delete particleColor; // Stellen Sie sicher, dass dies nur einmal pro `particleColor` geschieht.
	//particleColor = nullptr; // Verhindert Zugriff auf freigegebenen Speicher
}

void Node::color(glm::vec3 color)
{
	if (isLeaf)
	{
		particleColor.r = color.r;
		particleColor.g = color.g;
		particleColor.b = color.b;
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
void Node::gravity(Physics* phy, Particle& p, glm::dvec3& force, double softening, double a, double& potentialEngergy, double& calculations)
{

	if (particle.position != p.position)
	{
		glm::dvec3 delta = massCenter - p.position;

		double r = glm::length(delta);

		if (r > 0)
		{

			//normal newtonian gravity
			if (phy->PlummerSoftening)
			{
				//Plummer softening
				double epsilon0 = softening / std::sqrt(1.0 + std::pow(r / a, 2));
				double distance = glm::dot(delta, delta) + epsilon0 * epsilon0;
				//normal direct force
				double powN = 3.0 / 2.0;
				//G = 1
				glm::dvec3 Force = (mass * p.mass * delta) / std::pow(distance, powN);
				//std::cout << "Force: " << glm::length(Force) << std::endl;
				Particle p2 = Particle(massCenter, mass);
				potentialEngergy += p.calcPotentialEnergie(p2, 1, epsilon0, 0);
				force += Force;
			}
			else
			{
				double distance = glm::dot(delta, delta) + softening * softening;
				//normal direct force
				//G = 1
				double forceMagnitude = (mass * p.mass) / std::pow(distance, 2);
				//std::cout << forceMagnitude << std::endl;
				glm::dvec3 Force = forceMagnitude * glm::normalize(delta);
				Particle p2 = Particle(massCenter, mass);
				potentialEngergy += p.calcPotentialEnergie(p2, 1, softening, 0);
				force += Force;
			}


			calculations++;
		}
	}
}


void Node::gravitySPH(Physics* phy,Particle& p, Node* root, glm::dvec3& force, double softening, double a, double& potentialEngergy, double& calculations)
{

	if (particle.position != p.position)
	{
		glm::dvec3 delta = massCenter - p.position;

		double h = phy->h; // Glättungsradius
		double k = phy->k; // Steifheitskonstante für Druck
		double rho0 = phy->rh0; // Ruhe- oder Referenzdichte
		double mu = phy->mu;
		double uConst = phy->thermalConstant;

		double alpha = phy->alpha; // Viskositätskoeffizient alpha
		double beta = phy->beta;  // Viskositätskoeffizient beta
		double gamma = phy->gamma; // Spezifische Wärmekapazität für ideales Gas

		double r = sqrt(delta.x * delta.x + delta.y * delta.y + delta.z * delta.z);

		if (r > 0)
		{
			//normal newtonian gravity
			if (phy->PlummerSoftening)
			{
				//Plummer softening
				double epsilon0 = softening / std::sqrt(1.0 + std::pow(r / a, 2));
				double distance = glm::dot(delta, delta) + epsilon0 * epsilon0;
				//normal direct force
				double powN = 3.0 / 2.0;
				//G = 1
				glm::dvec3 Force = (mass * p.mass * delta) / std::pow(distance, powN);
				//std::cout << "Force: " << glm::length(Force) << std::endl;
				Particle p2 = Particle(massCenter, mass);
				potentialEngergy += p.calcPotentialEnergie(p2, 1, epsilon0, 0);
				force += Force;
			}
			else
			{
				double distance = glm::dot(delta,delta) + softening * softening;
				//normal direct force
				//G = 1
				double forceMagnitude = (mass * p.mass) / std::pow(distance, 2);
				//std::cout << forceMagnitude << std::endl;
				glm::dvec3 Force = forceMagnitude * glm::normalize(delta);
				Particle p2 = Particle(massCenter, mass);
				potentialEngergy += p.calcPotentialEnergie(p2, 1, softening, 0);
				force += Force;
			}
			
			//SPH
			if (r < 2 * h)
			{
				//Druckkraft
				// Dichte berechnen
				double density_i = p.density;
				//density of the node is already calculated
				// Druck berechnen
				double pressure_i = k * (density_i - rho0);
				double pressure = k * (density - rho0);
				// Gradient der Kernel-Funktion
				if (phy->adaptiveSmoothingLength) h = p.h;

				if (phy->normilizedPressureForce)
				{
					glm::dvec3 gradKernel = glm::normalize(delta) * MathFunctions::cubicSplineKernel(r, h);

					//(original)
					double pressureForce = - mass * (pressure_i / (density_i * density_i));

					glm::dvec3 vecPressureForce = pressureForce * gradKernel;
					// Kraft auf Partikel i anwenden
					force += vecPressureForce;
				}
				else
				{
					glm::dvec3 gradKernel = MathFunctions::gradientCubicSplineKernel(delta, h);

					//(original)
					double pressureForce = -mass * ((pressure_i / (density_i * density_i)) + (pressure / (density * density)));

					glm::dvec3 vecPressureForce = pressureForce * gradKernel;
					// Kraft auf Partikel i anwenden
					force -= vecPressureForce;
				}
				//Viskosen Kräfte
				//if both velocities are a rational number
				if (!std::isnan(glm::length(p.velocity)) && !std::isnan(glm::length(particle.velocity)))
				{
					if (phy->artificialViscosity)
					{
						// Mittlere Schallgeschwindigkeit
						double ci = sqrt(gamma * pressure_i / p.density);
						double cj = sqrt(gamma * pressure / density);
						double cij = (ci + cj) / 2.0;

						// Mittlere Dichte
						double density1 = p.density;
						double density2 = density;
						double rhoij = (density1 + density2) / 2.0;

						// Geschwindigkeitsdifferenz
						glm::dvec3 velocityDiff = p.velocity - particle.velocity;
						glm::dvec3 rij = p.position - massCenter;
						if (phy->adaptiveSmoothingLength) h = p.h;
						double mu_ij = h * glm::dot(velocityDiff, rij) / (glm::dot(rij, rij) + 0.01 * std::pow(h, 2));

						// Viskositätstensor Πij
						double Pi_ij = 0.0;
						if (glm::dot(velocityDiff, rij) < 0) {
							Pi_ij = (-alpha * cij * mu_ij + beta * (mu_ij * mu_ij)) / rhoij;
						}

						// Kernel-Funktion und deren Gradient
						double hi = h;
						double hj = h;
						if (phy->adaptiveSmoothingLength)
						{
							hi = p.h;
							hj = smoothingLength;
						}
						double hij = (hi + hj) / 2.0;
						glm::dvec3 gradW = MathFunctions::gradientCubicSplineKernel(rij, hij);

						double viscousForce = -mass * Pi_ij;
						glm::dvec3 vForce = viscousForce * gradW;
						//double forcelenght = sqrt(vForce.x * vForce.x + vForce.y * vForce.y + vForce.z * vForce.z);
						//std::cout << forcelenght << std::endl;
						force += vForce;
					}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					//Thermal Energy
					glm::dvec3 velocityDiff = p.velocity - particle.velocity;
					double distance = glm::length(p.position - massCenter);
					glm::dvec3 cubicSplineKernel = glm::normalize(delta) * MathFunctions::cubicSplineKernel(distance, h);
					double thermalEnergyChange = uConst * (pressure_i / p.density)* mass * glm::dot(velocityDiff,cubicSplineKernel);
					p.thermalEnergyChange += thermalEnergyChange;
				}
			}

			calculations++;
		}
	}
}

glm::dvec3 Node::calcForce(Physics* phy, Particle& p, Node* root, double softening, double a, double& potentialEngergy, double& calculations)
{
	glm::dvec3 force = { 0,0,0 };

	if (isLeaf)
	{
		if (phy->SPH)
		{
			//SPH
			gravitySPH(phy, p, root, force, softening, a, potentialEngergy, calculations);
		}
		else
		{
			//normal direct force
			gravity(phy, p, force, softening, a, potentialEngergy, calculations);
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
			if (phy->SPH)
			{
				//SPH
				gravitySPH(phy, p, root, force, softening, a, potentialEngergy, calculations);
			}
			else
			{
				//normal direct force
				gravity(phy, p, force, softening, a, potentialEngergy, calculations);
			}
		}
		else
		{
			// For all child nodes n
			for (Node* child : child)
			{
				if (child != nullptr)
				{
					force += child->calcForce(phy, p, root, softening, a, potentialEngergy, calculations);
				}
			}
		}
	}

	/* if(Physics::SPH){
		double h = 0.5e19; // Glättungsradius
		glm::dvec3 delta = massCenter - p.position;
		double r = glm::length(delta);

		if(r < 2*h){
			p.velocity *- 1.0;
		}
	} */
	//p.velocity *- 1.0;

	return force;
}

void Node::calcDensity(Particle& p, double h, double& medium, int& n)
{
	//choose the node in wich the particle is in and the radius of the node is the closest to h and than save the density of the node in the particle
	if (radius < h)
	{
		p.density = density;
		p.darkMatterDensity = darkMatterDensity;
		p.baryonicDensity = baryonicDensity;
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
			p.darkMatterDensity = darkMatterDensity;
			p.baryonicDensity = baryonicDensity;
			n++;
			medium += density;
		}
	}
}

void Node::insert(Particle& p)
{
	if (index < maxDepth)
	{
		if (mass == 0)
		{
			mass = p.mass;
			particle = p;
			particleColor = p.color;
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
				child[quadrant] = new Node(newCenter, newRadius, theta, index + 1, maxDepth, renderTree, glm::vec3(1, 0, 0));
			}
			child[quadrant]->insert(p);

			mass += p.mass;
			massCenter = (massCenter * (mass - p.mass) + p.position * p.mass) / mass;
			//dark and baryonic matter mass
			if (p.darkMatter)
			{
				darkMatterMass += p.mass;
			}
			else
			{
				baryonicMass += p.mass;
			}

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
		//dark and baryonic matter mass
		if (p.darkMatter)
		{
			darkMatterMass += p.mass;
		}
		else
		{
			baryonicMass += p.mass;
		}
		//std::cout << "max depth reached in index: " << index << std::endl;
	}
	//calc the density of the node
	density = mass / (4.0 / 3.0 * 3.14159265359 * radius * radius * radius);
	darkMatterDensity = darkMatterMass / (4.0 / 3.0 * 3.14159265359 * radius * radius * radius);
	baryonicDensity = baryonicMass / (4.0 / 3.0 * 3.14159265359 * radius * radius * radius);

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

void Node::calcH()
{
	if (isLeaf)
	{
		particle.h = 5e18;
	}
	else
	{
		for (Node* child : child)
		{
			if (child != nullptr)
			{
				child->calcH();
			}
		}
	}
}


bool Node::isInside(Particle& p)
{
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