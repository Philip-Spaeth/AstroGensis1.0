#ifndef OCTREE_H
#define OCTREE_H

#include <vector>
#include <glm.hpp>
#include "Node.h"
#include "Particle.h"


class Octree {
public:
    Octree(glm::dvec3 center, double halfSize);
    ~Octree();

    double theta = 0.5;

    void buildTree(std::vector<Particle> particles);
    bool isNodeFarAway(Particle& particle1,Particle& particle2, double theta);
    
	Node* root;
    glm::dvec3 center;

};

#endif // OCTREE_H