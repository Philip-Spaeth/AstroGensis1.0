#ifndef NODE_H
#define NODE_H

#include <vector>
#include <glm.hpp>
#include "Particle.h"
#include "Physics.h"

class Particle;
class Physics;

class Node 
{
public:
	Node(glm::dvec3 center, double radius, double theta, int index, int maxdepth, bool renderTree, glm::vec3 newparticleColor, int ParticlesSize);
	~Node();

	bool isInside(Particle* p);

	void insert(Particle* p);
	glm::dvec3 calcForce(Physics* phy, Particle* p, Node* root, double softening, double a, double& potentialEngergy, double& calculations);

	//SPH
	void gravitySPH(Physics* phy,Particle* p, Node* root, glm::dvec3& force, double softening, double a, double& potentialEngergy, double& calculations);

	void gravity(Physics* phy, Particle* p, glm::dvec3& force, double softening, double a, double& potentialEngergy, double& calculations);
    void calcMass();
	void calcH(Particle* p);
	void color(glm::vec3 color);

	Node* child[8] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };

	int index = 0;
	int particlesize = 0;
	int maxDepth;
	int depth = 5;
	bool isLeaf = false;
	glm::dvec3 center;
	glm::dvec3 massCenter;
	double mass = 0;
	double baryonicMass = 0;
	double darkMatterMass = 0;
	double radius = 0;
	double theta = 0;
	bool reservedVe = false;

	//SPH
	double density = 0;
	double h = 0;
	double pressure = 0;

	//just for color
	double baryonicDensity = 0;
	double darkMatterDensity = 0;
	void calcDensity(Particle* p,double h, double& medium, int& n);
	void calcDensity(double h, double& medium, int& n);

	bool renderTree = false;

	glm::vec3 particleColor;
	Particle particle;
	int currentIndex = 0;
	Particle* baryonicParticles[100000];
	bool particlePushed = false;

	void clear();

};

#endif