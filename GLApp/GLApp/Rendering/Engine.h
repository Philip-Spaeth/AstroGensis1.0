#ifndef ENGINE_H
#define ENGINE_H

#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Particle.h"
#include "Physics.h"
#include "FileManager.h"

class Physics;
class FileManager;

class Engine {
public:
    Engine();

    bool init(double physicsFaktor);
    void start(std::vector<std::vector<Particle>>& particles);
    void update(int index, std::vector<std::vector<Particle>>& particles);
    bool clean();

    GLFWwindow* window;

    bool isRunning = false;

private:

    double passedTime = 0;

    double globalScale = 1e-18;
    //double globalScale = 1e-18;

    bool BGstars = true;
    int amountOfStars = 1000;
    std::vector<glm::vec3> stars;

    glm::dvec3 cameraPosition;
    glm::dvec3 cameraFront;
    glm::dvec3 cameraUp;
    double cameraSpeed = 100;
    double rushSpeed = 1000;
    double cameraYaw;
    double cameraPitch;
    double cameraViewDistance = 1e15;
    glm::mat4 view;

    // Funktionen für Kamerasteuerung 
    void processMouseInput();
    void processInput();

    GLuint shaderProgram;
    GLuint VAO;
    void renderParticles(int index, std::vector<std::vector<Particle>>& particles);
    void checkShaderCompileStatus(GLuint shader, const char* shaderType);
    void checkShaderLinkStatus(GLuint program);
    void calcTime(glm::dvec3 position, int index);
    double faktor = -1;
    FileManager* fileManager;
    double random(double min, double max);
};

#endif