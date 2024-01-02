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
    void start();
    void update(int index);
    bool clean();

    GLFWwindow* window;
    Physics* physics;

    bool isRunning = false;

    double playSpeed = 1;
    double changeSpeed = 1;

    bool showDarkMatter = true;
    bool densityColor = true;

    double passedTime = 0;

    double globalScale = 1e-9;
    //double globalScale = 1e-18;
    void calculateGlobalScale();

    //render Tree
    const double theta = 0;
    std::vector<glm::vec4> positions;
    std::vector<glm::vec3> colors;
    std::vector<glm::vec3> densityColors;
private:

    bool BGstars = true;
    int amountOfStars = 1000;
    std::vector<glm::vec4> bgStars;

    bool tracks = false;

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
    bool shouldClose = false;
    GLuint VAO;
    GLuint instanceVBO;
    void renderParticles();
    void checkShaderCompileStatus(GLuint shader, const char* shaderType);
    void checkShaderLinkStatus(GLuint program);
    void calcTime(int index);
    void calcTime(glm::dvec3 position, int index);
    double faktor = -1;
    FileManager* fileManager;
    double random(double min, double max);
};

#endif