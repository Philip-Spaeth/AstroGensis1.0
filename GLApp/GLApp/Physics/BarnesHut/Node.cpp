#include "Node.h"
#include <iostream> 

Node::Node(glm::dvec3 center, double radius, double theta, int index, int maxdepth)
{
	this->center = center;
	this->radius = radius;
	this->theta = theta;
	this->index = index;
	this->maxDepth = maxdepth;
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


glm::dvec3 Node::calcForce(Particle& p, double softening, double& potentialEngergy, double& calculations)
{
	double G = 6.67408e-11;

	glm::dvec3 force = {0,0,0};

	if (isLeaf) 
	{
		if (particle.position != p.position)
		{
			glm::dvec3 delta = massCenter - p.position;
			double distance = glm::length(delta);
			if (distance > softening)
			{
				double forceMagnitude = (G * mass * p.mass) / (distance * distance);
				glm::dvec3 Force = forceMagnitude * glm::normalize(delta);
				force += Force;
				Particle p2 = Particle(massCenter, mass);
				potentialEngergy += p.calcPotentialEnergie(p2, G, softening, 0);
				calculations++;
			}
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
			if (r > softening)
			{
				glm::dvec3 delta = massCenter - p.position;
				double distance = glm::length(delta);
				double forceMagnitude = (G * mass * p.mass) / (distance * distance);
				glm::dvec3 Force = forceMagnitude * glm::normalize(delta);
				Particle p2 = Particle(massCenter, mass);
				potentialEngergy += p.calcPotentialEnergie(p2, G, softening, 0);
				force += Force;
				calculations++;
			}
		}
		else
		{
			// For all child nodes n
			for (Node* child : child) 
			{
				if(child != nullptr)
				{
					force += child->calcForce(p, softening, potentialEngergy, calculations);
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
				child[quadrant] = new Node(newCenter, newRadius, theta, index + 1, maxDepth);
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

