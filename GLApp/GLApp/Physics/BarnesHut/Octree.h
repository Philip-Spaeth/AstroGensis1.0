#ifndef OCTREE_H
#define OCTREE_H

#include <vector>
#include <glm.hpp>
#include "Node.h"
#include "Particle.h"


class Octree {
public:
    Octree(glm::dvec3 center, double radius, double theta, int maxDepth);
    ~Octree();

    void buildTree(std::vector<Particle>& particles);
    glm::dvec3 calculateForces(Particle& particle, double softening, double a, double& potentialEngergy, double& calculations);
    void clearTree();
private:

	Node* root;
    glm::dvec3 center;
    double radius;
    double theta;

    void insert(std::vector<Particle>& particles, int start, int end);
};

#endif // OCTREE_H