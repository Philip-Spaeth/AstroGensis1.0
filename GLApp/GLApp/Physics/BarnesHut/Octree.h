#ifndef OCTREE_H
#define OCTREE_H

#include <vector>
#include <glm.hpp>
#include "Node.h"
#include "Particle.h"
#include "Physics.h"

class Node;
class Physics;

class Octree {
public:
    Octree(glm::dvec3 center, double radius, double theta, int maxDepth);
    ~Octree();

    void buildTree(std::vector<Particle>& particles);
    void setColors();
    double calculateTotalMassInSphere(glm::dvec3 targetPosition, double radius);
    double calculateTotalMassInSphere(Node* currentNode, glm::dvec3 targetPosition, double radius);
    void calcdensity(Particle& p,double h, double& medium, int& n);
    glm::dvec3 calculateForces(Physics* phy, Particle& particle, double softening, double a, double& potentialEngergy, double& calculations);
    void calcH();
private:

	Node* root;
    glm::dvec3 center;
    double radius;
    double theta;

    void insert(std::vector<Particle>& particles, int start, int end);
};

#endif // OCTREE_H