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

    void buildTree(std::vector<Particle> particles);

    //double calculateGravitationForce();
    std::vector<Particle> getSummerizedParticles();

    double accuracy = 0.1;
private:
    double accuracyIndex;

	Node* root;
    glm::dvec3 center;

};

#endif // OCTREE_H