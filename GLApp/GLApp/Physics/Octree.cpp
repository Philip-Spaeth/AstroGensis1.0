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

	root = new Node(center, maxDistance, accuracyIndex);

    for (int p = 0; p < particles.size(); p++) 
    {
		// Es muss noch ein letzter Stoss durchgegeben werden, dass Partikel mitten drin ans Ende rutschen.
		root->InsertToNode(particles[p]);
    }

	root->bringParticleToEnd();
}

//double Octree::calculateGravitationForce()
//{
//	// Füge hier den Code für die Berechnung der Gravitationskraft hinzu (rekursiv durch den Baum gehen)
//	std::vector<Particle*> collectedParticles;
//	collectedParticles = root->collectChildren(collectedParticles);
//
//	glm::dvec3 totalForce(0.0, 0.0, 0.0);
//
//	for (int i = 0; i < collectedParticles.size(); i++)
//	{
//		for (int j = 0; j < collectedParticles.size(); j++)
//		{
//			if (i != j)
//			{
//				Particle* otherParticle = collectedParticles[j];
//				//collectedParticles[i]->force += collectedParticles[i]->calculateGravitationForce(*collectedParticles[j]);
//				glm::dvec3 force = collectedParticles[i]->calculateGravitationalForce(otherParticle, G, 0, deltaTime, k);
//				totalForce += force;
//			}
//		}
//	}
//
//
//	return 0.0;
//}

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