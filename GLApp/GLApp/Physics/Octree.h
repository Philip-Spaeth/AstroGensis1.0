#ifndef OCTREE_H
#define OCTREE_H

#include <vector>
#include <glm.hpp>
#include "Particle.h"

class Octree {
public:
    Octree(glm::dvec3 center, double halfSize);
    ~Octree();

    void buildTree(const std::vector<Particle>& particles);
    glm::dvec3 calculateGravitationalForce(const Particle& particle, double G, double softening);

private:
    // Definition der Struktur für einen Octree-Knoten
    struct OctreeNode {
        glm::dvec3 center;
        double halfSize;
        std::vector<Particle> particles;
        OctreeNode* children[8];  // Array für die acht Kinder
    };

    OctreeNode* root;

    // Private Funktionen für den Aufbau und die Berechnung des Octree
    void subdivideNode(OctreeNode* node);
    void insertParticle(OctreeNode* node, const Particle& particle);
    void calculateForceRecursive(OctreeNode* node, const Particle& particle, glm::dvec3& totalForce, double G, double softening);
};

#endif // OCTREE_H
