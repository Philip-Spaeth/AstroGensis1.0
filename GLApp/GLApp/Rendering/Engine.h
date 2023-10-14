#ifndef ENGINE_H
#define ENGINE_H

#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Particle.h"
#include "Physics.h"

class Engine {
public:
    Engine();

    bool init();
    void start(std::vector<std::vector<Particle>>& particles);
    void update(int deltaTime, std::vector<std::vector<Particle>>& particles);
    bool clean();

    GLFWwindow* window;
private:

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

};

#endif