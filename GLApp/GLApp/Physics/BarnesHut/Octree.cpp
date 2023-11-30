#include "Octree.h"

Octree::Octree(glm::dvec3 center, double halfSize) 
{
	// Erstelle hier den Wurzelknoten des Octrees
	this->center = center;
}

Octree::~Octree() {
    // Füge hier den Code für die Freigabe des Speichers hinzu (rekursiv durch den Baum gehen)
}

void Octree::buildTree(std::vector<Particle> particles) 
{
	// acuura
	accuracyIndex = particles.size() * accuracy;

	double maxDistance = 0;
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

	root = new Node(center, maxDistance * 2, accuracyIndex);

    for (int p = 0; p < particles.size(); p++) 
    {
		root->InsertToNode(particles[p].position, particles[p].mass, particles[p].velocity);
    }

	root->buildTree();
}

std::vector<Particle> Octree::getSummerizedParticles()
{
	std::vector<Particle*> collectedParticles;
	collectedParticles = root->collectChildren(collectedParticles);

	std::vector<Particle> summerizedParticles;
	for (int i = 0; i < collectedParticles.size(); i++)
	{
		summerizedParticles.push_back(*collectedParticles[i]);
	}

	return summerizedParticles;
}