#include "EllipticalGalaxy.h"
#include <gtc/matrix_transform.hpp> 
#include <gtx/transform.hpp>

void EllipticalGalaxy::S0(int startIndex, int endIndex, glm::dvec3 position, glm::dvec3 rotation, glm::dvec3 velocity, double size, std::vector<Particle>& particles)
{
    int particleSize = endIndex + 1 - startIndex;

    double galaxyRadius = 1e21 * size; // Radius der kugelförmigen Galaxie

    double totalMass = 2.8e40 * size; // Gesamtmasse der Galaxie
    double baryonicFraction = 1; // Anteil der baryonischen Materie
    double darkMatterFraction = 3; // Anteil der dunklen Materie

    double powNumber = 0.5; // Verteilungsparameter für baryonische Materie
    double darkPowNumber = 2; // Verteilungsparameter für dunkle Materie

    glm::mat4 transform = glm::mat4(1.0f); // Identity matrix

    // Apply rotations
    transform = glm::rotate(transform, (float)rotation.x, glm::vec3(1, 0, 0));
    transform = glm::rotate(transform, (float)rotation.y, glm::vec3(0, 1, 0));
    transform = glm::rotate(transform, (float)rotation.z, glm::vec3(0, 0, 1));

    for (int j = startIndex; j != endIndex; j++)
    {
        double theta = physics.random(0, glm::pi<double>());
        particles[j].angle = physics.random(0, 2 * glm::pi<double>());

        double random = physics.random(- baryonicFraction, darkMatterFraction);
        bool isDarkMatterParticle;
        if (random > 0)
        {
            isDarkMatterParticle = true;
        }
        else
        {
            isDarkMatterParticle = false;
        }


        double scaledI = std::pow(static_cast<double>(j - startIndex) / particleSize, 1.0 / (isDarkMatterParticle ? darkPowNumber : powNumber));
        double r = galaxyRadius * scaledI;

        double x = r * std::sin(theta) * std::cos(particles[j].angle);
        double y = r * std::sin(theta) * std::sin(particles[j].angle);
        double z = 0;

        double bulgeScale = 0.05;
        double diskScale = 0.03;
        double bulgeR = galaxyRadius / 10;
        // Berechnen der Z-Koordinate basierend auf der Position im Bulge oder in der Disk
        if (r < bulgeR)
        {
            z = (physics.gaussianRandom() * bulgeScale * galaxyRadius);
        }
        else
        {
            z = physics.gaussianRandom() * diskScale * galaxyRadius;
        }
        //distanz zum zetrum berechnen
        double distanceToCenter = glm::abs(glm::length(glm::dvec3(x, y, 0)));
        //null ausschließen um division durch null zu vehindern
        if (distanceToCenter == 0)
        {
            distanceToCenter = 1;
        }
        //position festlegen
        particles[j].position = glm::dvec3(x, y, z) + position;

        // Berechnung der Masse in der Kugel bis zum aktuellen Partikel
        double baryonicMassInSphere = baryonicFraction * totalMass * std::pow(distanceToCenter / galaxyRadius, powNumber);
        double darkMassInSphere = darkMatterFraction * totalMass * std::pow(distanceToCenter / galaxyRadius, darkPowNumber);
        double totalMassInSphere = baryonicMassInSphere + darkMassInSphere;

        double e = 1;

        //slwo down in bulge to cause caotiv movement so its more realistic
        if (distanceToCenter < bulgeR)
        {
            e = ((distanceToCenter * distanceToCenter)/ bulgeR) / bulgeR * (bulgeR / distanceToCenter);
            if (e > 1)
            {
                e = 1;
            }
        }
        
        // Berechnung der Geschwindigkeit
        double v = (std::sqrt(physics.G * totalMassInSphere / distanceToCenter) * e);

        particles[j].velocity = glm::dvec3(-v * std::sin(particles[j].angle), v * std::cos(particles[j].angle), 0) + velocity;
        particles[j].mass = totalMass / particleSize;
        particles[j].radius = 1;
        particles[j].darkMatter = isDarkMatterParticle;
        if(isDarkMatterParticle)particles[j].color = glm::vec3(0, 1000, 0);
        else particles[j].color = glm::vec3(1, 1, 1);

        glm::vec4 pos = glm::vec4(particles[j].position - position, 1.0);
        glm::vec4 vel = glm::vec4(particles[j].velocity, 1.0);

        pos = transform * pos;
        vel = transform * vel;

        particles[j].position = glm::dvec3(pos) + position;
        particles[j].velocity = glm::dvec3(vel);
    }
}


void EllipticalGalaxy::E0(int startIndex, int endIndex, glm::dvec3 position, glm::dvec3 rotation, glm::dvec3 velocity, double size, std::vector<Particle>& particles)
{
    int particleSize = endIndex + 1;

    double galaxyRadius = 1e21 * size; // Radius der Galaxie
    double totalMass = 1e40; // Gesamtmasse der Galaxie
    double mass = totalMass / particleSize; // Masse eines einzelnen Partikels

    int i = 0;

    // Erstellen einer elliptischen Galaxie
    for (int j = startIndex; j != endIndex; j++)
    {
        double theta = physics.random(0, glm::pi<double>());
        double phi = physics.random(0, 2 * glm::pi<double>());

        //distribution
        double scaledI = i / (double)particleSize;
        double faktor = 3;
        double r = galaxyRadius * std::pow(scaledI, faktor) * (faktor * faktor);

        double x = r * std::sin(theta) * std::cos(phi);
        double y = r * std::sin(theta) * std::sin(phi);
        double z = r * std::cos(theta); // Für eine dreidimensionale Struktur

        particles[j].position = glm::dvec3(x, y, z) + position;

        // Geschwindigkeitsberechnung basierend auf der Gravitation
        double distanceToCenter = glm::length(particles[j].position - position);
        double massInSphere = totalMass * std::pow(scaledI, 3); // Masse innerhalb der Kugel
        double v = std::sqrt(physics.G * massInSphere / distanceToCenter);

        particles[j].velocity = glm::dvec3(-v * std::sin(phi), v * std::cos(phi), 0) + velocity;
        particles[j].mass = mass;
        particles[j].radius = 1;
        i++;
    }
}