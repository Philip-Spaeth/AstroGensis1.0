#include "Node.h"
#include "Particle.h"

Node::Node(glm::dvec3 center, double halfsize, double accuracyIndex)
{
	this->center = center;
	//particles = std::vector<Particle>();
	//halfSize = 0;
	this->halfSize = halfsize;
	this->accuracyIndex = accuracyIndex;
}

void Node::InsertToNode(Particle& particle)
{
	// berechne accuracy


	particles.push_back(particle);
	if (particles.size() > accuracyIndex)
	{
		//split the node
		if (!childrenCreated) {
			glm::dvec3 newCenter = center;
			newCenter.x += this->halfSize / 2;
			newCenter.y += this->halfSize / 2;
			newCenter.z += this->halfSize / 2;
			this->children[0] = new Node(newCenter, this->halfSize/2, this->accuracyIndex);
			newCenter.x -= halfSize;
			this->children[1] = new Node(newCenter, this->halfSize / 2, this->accuracyIndex);
			newCenter.y -= halfSize;
			this->children[2] = new Node(newCenter, this->halfSize / 2, this->accuracyIndex);
			newCenter.x += halfSize;
			this->children[3] = new Node(newCenter, this->halfSize / 2, this->accuracyIndex);
			newCenter.z -= halfSize;
			this->children[4] = new Node(newCenter, this->halfSize / 2, this->accuracyIndex);
			newCenter.x -= halfSize;
			this->children[5] = new Node(newCenter, this->halfSize / 2, this->accuracyIndex);
			newCenter.y += halfSize;
			this->children[6] = new Node(newCenter, this->halfSize / 2, this->accuracyIndex);
			newCenter.x += halfSize;
			this->children[7] = new Node(newCenter, this->halfSize / 2, this->accuracyIndex);

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

void Node::calculateMassAndCenter()
{
	// Info an Endpunkte weitergeben
	if (this->childrenCreated)
	{
		for (int i = 0; i < 8; i++)
		{
			this->children[i]->calculateMassAndCenter();
		}
		this->center /= this->mass;
	}
	// Berechnung der Masse und des Schwerpunktes
	else
	{
		for (int i = 0; i < this->particles.size(); i++)
		{
			this->mass += this->particles[i].mass;
			this->globalCenter += this->particles[i].position * this->particles[i].mass;
		}
		this->globalCenter /= this->mass;
	}

	// Zusammenfassen der Massen und Schwerpunkte
	this->centerOfMassChild = new Particle(this->globalCenter.x, this->globalCenter.y, this->globalCenter.z);
	this->centerOfMassChild->mass = this->mass;
	this->centerOfMassChild->radius = this->particles[0].radius;
	this->centerOfMassChild->color = this->particles[0].color;
}

std::vector<Particle*> Node::collectChildren(std::vector<Particle*> collectedParticles)
{
	if (this->childrenCreated)
	{
		for (int i = 0; i < 8; i++)
		{
			this->children[i]->collectChildren(collectedParticles);
		}
	}
	else
	{
		collectedParticles.push_back(this->centerOfMassChild);
	}
	return collectedParticles;
}


void Node::bringParticleToEnd()
{
	if (this->childrenCreated)
	{
		this->accuracyIndex = 0;
		for (int i = 0; i < 8; i++)
		{
			if(this->particles.size() + this->children[i]->particles.size() > this->accuracyIndex)
				// Index vom anderen so aendern, dass Partikel aufgeschoben werden koennen.
				this->children[i]->accuracyIndex = this->accuracyIndex - this->particles.size();
		}
		// Partikel an Endpunkt weitergeben
		while (this->particles.size() > this->accuracyIndex) {
			Particle particleToMove = this->particles[this->particles.size() - 1];
			// Der Partikel muss entfernt werden, da er von InsertToNode wieder hinzugefuegt wird.
			this->particles.pop_back();
			this->InsertToNode(particleToMove);
		}

		for (int i = 0; i < 8; i++)
		{
			this->children[i]->bringParticleToEnd();
		}
	}
	else {
		int a = 0;
	}
}