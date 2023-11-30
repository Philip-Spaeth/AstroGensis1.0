#include "Octree.h"

Octree::Octree(glm::dvec3 center, double halfSize) 
{
	// Erstelle hier den Wurzelknoten des Octrees
	this->center = center;
	root = new Node(center, halfSize);
}

Octree::~Octree() {
    // Füge hier den Code für die Freigabe des Speichers hinzu (rekursiv durch den Baum gehen)
}

void Octree::buildTree(std::vector<Particle> particles) 
{
	double maxDistance = -10e10;
	for (int i = 0; i < particles.size(); i++) 
    {
		double distance = sqrt(pow(particles[i].position.x, 2) + pow(particles[i].position.y, 2) + pow(particles[i].position.z, 2));
		if (distance > maxDistance) {
			maxDistance = distance;
		}
	}
	// Falls maxDistance <0, dann umkehren, weil wir betrag brauchen
	if (maxDistance < 0) {
		maxDistance *= -1;
	}

	root = new Node(center, maxDistance);

    for (int p = 0; p < particles.size(); p++) 
    {
		root->InsertToNode(particles[p]);
    }
}

// Inside Octree class
bool Octree::isNodeFarAway(Particle& particle1, Particle& particle2, double theta) {
	double distance = glm::length(particle1.position - particle2.position);
	double size = root->halfSize;

	return (size / distance) < theta;
}