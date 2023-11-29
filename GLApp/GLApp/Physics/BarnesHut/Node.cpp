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

void Node::InsertToNode(glm::dvec3 position, double mass)
{
	positions.push_back(position);
	masses.push_back(mass);
}

void Node::buildTree()
{
	if (positions.size() > accuracyIndex)
	{
		// create children
		createChildren();

		// insert particles into children
		sortParticlesIntoChildren();

		// build tree for children
		for (int i = 0; i < 8; i++)
		{
			this->children[i]->buildTree();
		}
	}
	else
	{
		// calculate mass and center
		//calculateMassAndCenter();
	}

}

void Node::createChildren() {
	for (int i = 0; i < 8; i++) {
		glm::dvec3 newCenter = center;
		newCenter.x += this->halfSize / 2;
		newCenter.y += this->halfSize / 2;
		newCenter.z += this->halfSize / 2;
		this->children[0] = new Node(newCenter, this->halfSize / 2, this->accuracyIndex);
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
	}
	this->childrenCreated = true;
}

void Node::sortParticlesIntoChildren() {
	while(positions.size() > 0) {
		/*for (int j = 0; j < 8; j++) {
			if (children[j]->isInside(positions[i])) {
				children[j]->InsertToNode(this->positions[i], this->masses[i]);
				positions.erase(positions.begin());
				masses.erase(masses.begin());
				break;
			}
		}*/

		if (this->positions[0].x < this->center.x)
		{
			if (this->positions[0].y < this->center.y)
			{
				if (this->positions[0].z < this->center.z)
				{
					this->children[0]->InsertToNode(this->positions[0], masses[0]);
				}
				else
				{
					this->children[1]->InsertToNode(this->positions[0], masses[0]);
				}
			}
			else
			{
				if (this->positions[0].z < this->center.z)
				{
					this->children[2]->InsertToNode(this->positions[0], masses[0]);
				}
				else
				{
					this->children[3]->InsertToNode(this->positions[0], masses[0]);
				}
			}
		}
		else
		{
			if (this->positions[0].y < this->center.y)
			{
				if (this->positions[0].z < this->center.z)
				{
					this->children[4]->InsertToNode(this->positions[0], masses[0]);
				}
				else
				{
					this->children[5]->InsertToNode(this->positions[0], masses[0]);
				}
			}
			else
			{
				if (this->positions[0].z < this->center.z)
				{
					this->children[6]->InsertToNode(this->positions[0], masses[0]);
				}
				else
				{
					this->children[7]->InsertToNode(this->positions[0], masses[0]);
				}
			}
		}
		this->positions.erase(this->positions.begin());
		this->masses.erase(this->masses.begin());
	}
	
}

bool Node::isInside(glm::dvec3 position) {
	// checken, ob Position innerhalb der Node liegt. Drei dimensionaler Raum. Position kann auch negativ sein.

	glm::dvec3 minBound = this->center - glm::dvec3(this->halfSize / 2, this->halfSize / 2, this->halfSize / 2);
	glm::dvec3 maxBound = this->center + glm::dvec3(this->halfSize / 2, this->halfSize / 2, this->halfSize / 2);

	if((position.x >= minBound.x && position.x <= maxBound.x) &&
		(position.y >= minBound.y && position.y <= maxBound.y) &&
		(position.z >= minBound.z && position.z <= maxBound.z)) {

		return true;
	}
	return false;


	/*if (position.x >= this->center.x - halfSize / 2 && position.x <= this->center.x + halfSize / 2 &&
		position.y >= this->center.y - halfSize / 2 && position.y <= this->center.y + halfSize / 2 &&
		position.z >= this->center.z - halfSize / 2 && position.z <= this->center.z + halfSize / 2) {
		return true;
	}
	return false;*/
}