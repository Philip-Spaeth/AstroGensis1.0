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
#include <Windows.h>
#include <debugapi.h>
#include <iostream>
#include <string>


int main()
{
    Physics physics;

    std::vector<std::vector<Particle>> particles(physics.numTimeSteps);

    physics.Calc(particles);

    std::cout << std::endl;
    std::cout << "Press enter to start" << std::endl;

    std::string input;
    std::getline(std::cin, input);

    Engine engine;

    if (!engine.init(physics.deltaTime)) {
        std::cerr << "Engine initialization failed." << std::endl;
        return -1;
    }

    engine.start(particles);

    double lastFrameTime = glfwGetTime(); // Zeit des letzten Frames
    double frameTime; // Zeitdauer eines Frames

    int frameCount = 0;
    double secondCounter = 0.0;
    int counter = 0;

    // Haupt-Render-Schleife
    while (!glfwWindowShouldClose(engine.window))
    {
        // check for exit Programm with Key ESC
        if (GetAsyncKeyState(27) & 0x8000)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            OutputDebugString(L"ESC KEY\n");
            glfwSetWindowShouldClose(engine.window, true);
        }

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
            // update particles
            engine.update(counter, particles);
            // add time when engine is running
            if (engine.isRunning) 
            {
                counter++;
            }
        }


        frameCount++;
        secondCounter += frameTime;

        // Wenn eine Sekunde vergangen ist, zeigen Sie die FPS an
        if (secondCounter >= 1.0)
        {
            char numStr[20]; // Ein char-Array zur Speicherung der Zeichenkette
            // Verwende sprintf, um die Ganzzahl in das char-Array umzuwandeln
            snprintf(numStr, sizeof(numStr), "%d", frameCount);

            //std::cout << "Die umgewandelte Zeichenkette: " << numStr << std::endl;
            strcat(numStr, " FPS");

            glfwSetWindowTitle(engine.window, numStr);
            //std::cout << "FPS: " << frameCount << std::endl;
            frameCount = 0;
            secondCounter = 0.0;
        }
    }

    // Beenden Sie GLFW
    engine.clean();
    glfwTerminate();
    return 0;
}