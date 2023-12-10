#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <cmath>
#include "Particle.h"

//  Nur unter Windows
#ifdef WIN32
#include "Engine.h"
#include <Windows.h>
#include <debugapi.h>
#endif

#include "Physics.h"
#include <iostream>
#include <string>
#include "FileManager.h"


#ifdef WIN32
int main()
{
    Physics physics;

    physics.Calc();

    std::cout << std::endl;
    std::cout << "Press enter to start" << std::endl;

    std::string input;
    std::getline(std::cin, input);

    Engine engine;
    FileManager* fileManager = new FileManager();

    if (!engine.init(physics.deltaTime)) {
        std::cerr << "Engine initialization failed." << std::endl;
        return -1;
    }

    engine.start();

    double lastFrameTime = glfwGetTime(); // Zeit des letzten Frames
    double frameTime; // Zeitdauer eines Frames

    int frameCount = 0;
    double secondCounter = 0.0;
    int counter = 0;


    std::vector<Particle> currentParticles;

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

        // Rufen Sie die update-Funktion auf und �bergeben Sie die Zeitdauer eines Frames
        if (frameTime < 1.0 / TARGET_FPS)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>((1.0 / TARGET_FPS - frameTime) * 1000)));
        }

        fileManager->loadParticles(counter, engine.positions, engine.colors);

        // update particles
        engine.update(counter);
        // add time when engine is running
        if (engine.isRunning)
        {
            if (counter >= 0 && counter <= physics.numTimeSteps - 1)
            {
                counter = counter + engine.playSpeed;
            }
        }
        if (counter >= physics.numTimeSteps - 1)
        {
			counter = physics.numTimeSteps - 1;
            engine.playSpeed = 0;
		}
        if (counter < 0)
        {
            counter = 0;
            engine.playSpeed= 0;
        }

        //restart if R is pressed
        if (GetAsyncKeyState(82) & 0x8000)
        {
			counter = 0;
			engine.playSpeed = 0;
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
            strcat_s(numStr, " FPS");

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

#else

int main()
{
    Physics physics;

    physics.Calc();

    std::cout << std::endl;
    std::cout << "Press enter to start" << std::endl;
}

#endif