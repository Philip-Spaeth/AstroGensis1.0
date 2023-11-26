#include "Octree.h"

Octree::Octree(glm::dvec3 center, double halfSize) {
    root = new OctreeNode;
    root->center = center;
    root->halfSize = halfSize;
    for (int i = 0; i < 8; ++i) {
        root->children[i] = nullptr;
    }
}

Octree::~Octree() {
    // Füge hier den Code für die Freigabe des Speichers hinzu (rekursiv durch den Baum gehen)
}

void Octree::buildTree(const std::vector<Particle>& particles) {
    // Füge hier den Code für den Aufbau des Octrees basierend auf den Partikeln hinzu
    for (const auto& particle : particles) {
        insertParticle(root, particle);
    }
}

glm::dvec3 Octree::calculateGravitationalForce(const Particle& particle, double G, double softening) {
    glm::dvec3 totalForce(0.0, 0.0, 0.0);
    calculateForceRecursive(root, particle, totalForce, G, softening);
    return totalForce;
}

void Octree::subdivideNode(OctreeNode* node) {
    // Füge hier den Code für die Unterteilung eines Knotens in acht Kinder hinzu
    double newHalfSize = node->halfSize * 0.5;
    for (int i = 0; i < 8; ++i) {
        glm::dvec3 newCenter = node->center;
        newCenter.x += ((i & 1) ? 1 : -1) * newHalfSize;
        newCenter.y += ((i & 2) ? 1 : -1) * newHalfSize;
        newCenter.z += ((i & 4) ? 1 : -1) * newHalfSize;

        node->children[i] = new OctreeNode;
        node->children[i]->center = newCenter;
        node->children[i]->halfSize = newHalfSize;
        for (int j = 0; j < 8; ++j) {
            node->children[i]->children[j] = nullptr;
        }
    }
}

void Octree::insertParticle(OctreeNode* node, const Particle& particle) {
    // Füge hier den Code zum Einfügen einer Partikel in den Octree hinzu
    if (node->children[0] == nullptr) {
        // Der Knoten hat noch keine Kinder, also füge die Partikel direkt hinzu
        node->particles.push_back(particle);

        // Wenn die maximale Anzahl von Partikeln pro Knoten erreicht ist, unterteile den Knoten
        if (node->particles.size() > 10) {
            subdivideNode(node);
            for (const auto& p : node->particles) {
                insertParticle(node, p);
            }
            node->particles.clear();  // Lösche die Partikel im aktuellen Knoten
        }
    }
    else {
        // Der Knoten hat Kinder, füge die Partikel in das entsprechende Kind ein
        for (int i = 0; i < 8; ++i) {
            if (particle.position.x > node->children[i]->center.x - node->children[i]->halfSize &&
                particle.position.x < node->children[i]->center.x + node->children[i]->halfSize &&
                particle.position.y > node->children[i]->center.y - node->children[i]->halfSize &&
                particle.position.y < node->children[i]->center.y + node->children[i]->halfSize &&
                particle.position.z > node->children[i]->center.z - node->children[i]->halfSize &&
                particle.position.z < node->children[i]->center.z + node->children[i]->halfSize) {
                insertParticle(node->children[i], particle);
                return;
            }
        }
    }
}

void Octree::calculateForceRecursive(OctreeNode* node, const Particle& particle, glm::dvec3& totalForce, double G, double softening) {
    // Füge hier den Code für die rekursive Berechnung der Gravitationskraft hinzu
    glm::dvec3 delta = node->center - particle.position;
    double distance = glm::length(delta);

    // Verhindere eine Division durch Null
    if (distance == 0.0) {
        return;
    }

    if (node->children[0] == nullptr) {
        // Der Knoten hat keine Kinder, also berechne die Gravitationskraft
        double forceMagnitude = (G * particle.mass * 1.0) / (distance * distance + softening);
        totalForce += forceMagnitude * glm::normalize(delta);
    }
    else {
        // Der Knoten hat Kinder, überprüfe, ob die Distanz zu diesem Knoten groß genug ist
        if (node->halfSize / distance < 0.5) {
            // Verwende den gesamten Knoten als einzelnes Teilchen
            double forceMagnitude = (G * particle.mass * 1.0) / (distance * distance + softening);
            totalForce += forceMagnitude * glm::normalize(delta);
        }
        else {
            // Durchsuche die Kinder des Knotens rekursiv
            for (int i = 0; i < 8; ++i) {
                calculateForceRecursive(node->children[i], particle, totalForce, G, softening);
            }
        }
    }
}
