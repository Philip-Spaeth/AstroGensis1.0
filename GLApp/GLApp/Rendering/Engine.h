#ifndef ENGINE_H
#define ENGINE_H

#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Particle.h"
#include "Physics.h"

class Physics;

class Engine {
public:
    Engine();

    bool init(double physicsFaktor);
    void start(std::vector<std::vector<Particle>>& particles);
    void update(int deltaTime, std::vector<std::vector<Particle>>& particles);
    bool clean();

    GLFWwindow* window;

private:

    double passedTime = 0;

    double globalScale = 1e-9;
    //double globalScale = 1e-18;

    glm::vec3 cameraPosition;
    glm::vec3 cameraFront;
    glm::vec3 cameraUp;
    float cameraSpeed = 100;
    float rushSpeed = 1000;
    float cameraYaw;
    float cameraPitch;
    glm::mat4 view;

    // Funktionen für Kamerasteuerung 
    void processMouseInput();
    void processInput();

    GLuint shaderProgram;
    GLuint VAO;
    void renderParticles(int deltaTime, std::vector<std::vector<Particle>>& particles);
    void checkShaderCompileStatus(GLuint shader, const char* shaderType);
    void checkShaderLinkStatus(GLuint program);
    double faktor = -1;
};

#endif