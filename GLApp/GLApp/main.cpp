#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <cmath>
#include "Particle.h"
#include "Engine.h"
#include "Physics.h"


int main()
{
    Physics physics;

    std::vector<std::vector<Particle>> particles(physics.numTimeSteps);

    physics.Calc(particles);

    Engine engine;

    if (!engine.init(physics.faktor)) {
        std::cerr << "Engine initialization failed." << std::endl;
        return -1;
    }

    engine.start(particles);

    double lastFrameTime = glfwGetTime(); // Zeit des letzten Frames
    double frameTime; // Zeitdauer eines Frames

    int frameCount = 0; // Anzahl der Frames in einer Sekunde
    double secondCounter = 0.0; // Zähler für Sekunden
    int counter = 0;

    // Haupt-Render-Schleife
    while (!glfwWindowShouldClose(engine.window))
    {
        double currentFrameTime = glfwGetTime();
        frameTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;

        // Rufen Sie die update-Funktion auf und übergeben Sie die Zeitdauer eines Frames
        if (frameTime < 1.0 / TARGET_FPS)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>((1.0 / TARGET_FPS - frameTime) * 1000)));
        }


        if (counter < physics.numTimeSteps)
        {
            engine.update(counter, particles);
            counter++;
        }


        frameCount++;
        secondCounter += frameTime;

        // Wenn eine Sekunde vergangen ist, zeigen Sie die FPS an
        if (secondCounter >= 1.0)
        {
            std::cout << "FPS: " << frameCount << std::endl;
            frameCount = 0;
            secondCounter = 0.0;
        }
    }

    // Beenden Sie GLFW
    engine.clean();
    glfwTerminate();
    return 0;
}