#include "Node.h"
#include "Particle.h"

Node::Node(glm::dvec3 center, double halfsize)
{
	this->center = center;
	//particles = std::vector<Particle>();
	//halfSize = 0;
	this->halfSize = halfsize;
}

void Node::InsertToNode(Particle& particle)
{
	particles.push_back(particle);
	if (particles.size() > 1)
	{
		//split the node
		if (!childrenCreated) {
			glm::dvec3 newCenter = center;
			newCenter.x += this->halfSize / 2;
			newCenter.y += this->halfSize / 2;
			newCenter.z += this->halfSize / 2;
			this->children[0] = new Node(newCenter, this->halfSize/2);
			newCenter.x -= halfSize;
			this->children[1] = new Node(newCenter, this->halfSize / 2);
			newCenter.y -= halfSize;
			this->children[2] = new Node(newCenter, this->halfSize / 2);
			newCenter.x += halfSize;
			this->children[3] = new Node(newCenter, this->halfSize / 2);
			newCenter.z -= halfSize;
			this->children[4] = new Node(newCenter, this->halfSize / 2);
			newCenter.x -= halfSize;
			this->children[5] = new Node(newCenter, this->halfSize / 2);
			newCenter.y += halfSize;
			this->children[6] = new Node(newCenter, this->halfSize / 2);
			newCenter.x += halfSize;
			this->children[7] = new Node(newCenter, this->halfSize / 2);

			this->childrenCreated = true;
		}

		//sort the particle in the right children Node
		if (particle.position.x < this->center.x)
		{
			if (particle.position.y < this->center.y)
			{
				if (particle.position.z < this->center.z)
				{
					this->children[0]->InsertToNode(particle);
				}
				else
				{
					this->children[1]->InsertToNode(particle);
				}
			}
			else
			{
				if (particle.position.z < this->center.z)
				{
					this->children[2]->InsertToNode(particle);
				}
				else
				{
					this->children[3]->InsertToNode(particle);
				}
			}
		}
		else
		{
			if (particle.position.y < this->center.y)
			{
				if (particle.position.z < this->center.z)
				{
					this->children[4]->InsertToNode(particle);
				}
				else
				{
					this->children[5]->InsertToNode(particle);
				}
			}
			else
			{
				if (particle.position.z < this->center.z)
				{
					this->children[6]->InsertToNode(particle);
				}
				else
				{
					this->children[7]->InsertToNode(particle);
				}
			}
		}
		this->particles.pop_back();
	}
}

glm::dvec3 Node::calculateGravitationalForceBarnesHut(Particle& particle, double G, double softening, double deltaTime, double theta) 
{
	glm::dvec3 direction = particle.position - center;
	double distance = glm::length(direction) + softening;
	double forceMagnitude = (G * mass * particle.mass) / (distance * distance * distance);

	// If the multipole acceptance criteria are met, use the Barnes-Hut approximation
	if ((halfSize / distance) < theta) {
		return forceMagnitude * glm::normalize(direction);
	}

	// If not, aggregate forces from the node's children
	glm::dvec3 totalForce(0.0, 0.0, 0.0);
	for (int i = 0; i < 8; ++i) {
		if (children[i]) 
		{
			totalForce += children[i]->calculateGravitationalForceBarnesHut(particle, G, softening, deltaTime, theta);
		}
	}

	return totalForce;
}
