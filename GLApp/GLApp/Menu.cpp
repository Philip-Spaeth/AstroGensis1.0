#include "Menu.h"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "Physics.h"
#include <iostream>
#include <string.h>
#include "FileManager.h"

//  Nur unter Windows
#include "Engine.h"
#ifdef WIN32
#include <Windows.h>
#include <debugapi.h>
#endif

using namespace std;
namespace fs = std::filesystem;

void Menu::startMenu()
{
	cout << "---------------------------- N-Body Simulation ---------------------------" << endl;
	cout << endl;
	cout << "Please select an option from the menu." << endl;
	cout << endl;
	cout << "[1] Calculate a Simulation" << endl;
	cout << "[2] 3D Live View of a Simulation" << endl;
	cout << "[3] Render a Video based on a Simulation\n" << endl;

	// Get the user's input
	int choice;
	cin >> choice;

	// Check if the user's input is valid
	while (choice < 1 || choice > 3)
	{
		cout << "Invalid input. Please enter a valid option." << endl;
		cin >> choice;
	}

	// Call the appropriate function based on the user's input
	switch (choice)
	{
		case 1:
			calculateSimulation();
			break;
		case 2:
			viewSimulation();
			break;
		case 3:
			renderSimulation();
			break;
	}
}

void Menu::calculateSimulation()
{
	Physics* physics = new Physics(dataFolder);
	physics->Calc();
	dataFolder = physics->dataFolder;

    cout << "Simulation has been calculated. Press ENTER to return to the main menu." << endl;
    
    //clear console
    std::cin.get();
	startMenu();
}

void Menu::viewSimulation()
{
	//get the DataFolder
    double deltaTime = 1;
    int numParticle = 1;
    int numTimeSteps = 1;

    std::string input;
    std::getline(std::cin, input);

    while (true)
    {
        std::string dataPath = "Data"; // Pfad zum Data-Ordner
        std::cout << endl;
        std::cout << "Available simulations: \n\n";

        // Überprüfen, ob das Verzeichnis existiert und darauf zugegriffen werden kann
        if (fs::exists(dataPath) && fs::is_directory(dataPath)) {
            int i = 1;
            for (const auto& entry : fs::directory_iterator(dataPath)) {
                std::cout << "[" << i << "] " << entry.path().filename() << std::endl;
                i++;
            }
        }
        else {
            std::cout << "No available simulations. The 'Data' folder is missing or inaccessible.\n";
        }


        //check wich folder to use based on the i number from above
        std::string Input;
        std::getline(std::cin, Input);
        if (Input == "")
        {
            break;
        }
        else
        {
            int i = std::stoi(Input);
            int j = 1;
            for (const auto& entry : fs::directory_iterator("Data"))
            {
                if (i == j)
                {
                    dataFolder = entry.path().filename().string();

                    // Get the number of timesteps and the particlesize from the info.txt file, separated by ';'
                    std::string infoFile = "Data/" + dataFolder + "/info.txt";
                    std::ifstream file(infoFile);
                    if (!file.is_open()) {
                        std::cerr << "Error opening file: " << infoFile << std::endl;
                        break;
                    }

                    std::string line;
                    // read the first line
                    if (std::getline(file, line)) {
                        try {
                            numTimeSteps = std::stoi(line);
                        }
                        catch (const std::invalid_argument& e) {
                            std::cerr << "Invalid number format for time steps: " << line << std::endl;
                            file.close();
                            break;
                        }
                    }
                    // read the second line
                    if (std::getline(file, line)) {
                        try {
                            numParticle = std::stoi(line);
                        }
                        catch (const std::invalid_argument& e) {
                            std::cerr << "Invalid number format for particle size: " << line << std::endl;
                            file.close();
                            break;
                        }
                    }
                    //read the third line
                    if (std::getline(file, line)) {
						try {
							deltaTime = std::stod(line);
						}
						catch (const std::invalid_argument& e) {
							std::cerr << "Invalid number format for delta time: " << line << std::endl;
							file.close();
							break;
						}
					}

                    file.close();
                    break;
                }
                j++;
            }
            break;
        }
    }
    Physics* physics = new Physics(dataFolder);
    if (physics->configFile) physics->config();
    physics->deltaTime = deltaTime;
    physics->particlesSize = numParticle;
    physics->numTimeSteps = numTimeSteps;
	
	//Remder the data
    Engine engine(dataFolder);
    FileManager* fileManager = new FileManager(dataFolder);

    if (!engine.init(physics->deltaTime)) {
        std::cerr << "Engine initialization failed." << std::endl;
        return;
    }

    engine.start(physics);

    double lastFrameTime = glfwGetTime(); // Zeit des letzten Frames
    double frameTime; // Zeitdauer eines Frames

    int frameCount = 0;
    double secondCounter = 0.0;
    int counter = 0;


    std::vector<Particle> currentParticles;

    // Haupt-Render-Schleife
    while (!glfwWindowShouldClose(engine.window))
    {
        #ifdef WIN32
        // check for exit Programm with Key ESC
        if (GetAsyncKeyState(27) & 0x8000)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            OutputDebugString(L"ESC KEY\n");
            glfwSetWindowShouldClose(engine.window, true);
        }
        /*
        #else
        if (getch() == 27) // ESC
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            //OutputDebugString(L"ESC KEY\n");
            glfwSetWindowShouldClose(engine.window, true);
        }
        */
        #endif

        double currentFrameTime = glfwGetTime();
        frameTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;

        // Rufen Sie die update-Funktion auf und �bergeben Sie die Zeitdauer eines Frames
        if (frameTime < 1.0 / TARGET_FPS)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>((1.0 / TARGET_FPS - frameTime) * 1000)));
        }

        fileManager->loadParticles(physics, counter, engine.positions, engine.colors, engine.densityColors, engine.maxNumberOfParticles);

        // update particles
        engine.update(counter);
        // add time when engine is running
        if (engine.isRunning)
        {
            if (counter >= 0 && counter <= physics->numTimeSteps - 1)
            {
                counter = counter + engine.playSpeed;
            }
        }
        if (counter >= physics->numTimeSteps - 1)
        {
            counter = physics->numTimeSteps - 1;
            engine.playSpeed = 0;
        }
        if (counter < 0)
        {
            counter = 0;
            engine.playSpeed = 0;
        }

        #ifdef WIN32
        //restart if R is pressed
        if (GetAsyncKeyState(82) & 0x8000)
        {
            counter = 0;
            engine.playSpeed = 0;
        }
        /*
        #else
        if (getch() == 82)
        {
            counter = 0;
            engine.playSpeed = 0;
        }
        */
        #endif

        frameCount++;
        secondCounter += frameTime;

        // Wenn eine Sekunde vergangen ist, zeigen Sie die FPS an
        if (secondCounter >= 1.0)
        {
            char numStr[20]; // Ein char-Array zur Speicherung der Zeichenkette
            // Verwende sprintf, um die Ganzzahl in das char-Array umzuwandeln
            snprintf(numStr, sizeof(numStr), "%d", frameCount);

            //std::cout << "Die umgewandelte Zeichenkette: " << numStr << std::endl;
            #ifdef WIN32
            strcat_s(numStr, " FPS");
            #else
            strcat(numStr, " FPS");
            #endif

            glfwSetWindowTitle(engine.window, numStr);
            //std::cout << "FPS: " << frameCount << std::endl;
            frameCount = 0;
            secondCounter = 0.0;
        }

        // Wenn F11 gedrückt wird fullstreen mit guter auflösung
        // Wenn F11 gedrückt wird, schalten Sie in den Vollbildmodus um
        if (glfwGetKey(engine.window, GLFW_KEY_F11) == GLFW_PRESS) 
        {
            // Speichern Sie die aktuelle Monitor-Information
            GLFWmonitor* monitor = glfwGetPrimaryMonitor();
            const GLFWvidmode* mode = glfwGetVideoMode(monitor);

            // Prüfen Sie den aktuellen Vollbild-Status des Fensters
            if (glfwGetWindowMonitor(engine.window) == NULL) {
                // Wenn das Fenster nicht im Vollbildmodus ist, schalten Sie in den Vollbildmodus
                glfwSetWindowMonitor(engine.window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
                engine.framebuffer_size_callback(engine.window, mode->width, mode->height);
            }
            else {
                // Wenn das Fenster bereits im Vollbildmodus ist, schalten Sie in den Fenstermodus
                glfwSetWindowMonitor(engine.window, NULL, 100, 100, 1200, 800, mode->refreshRate);
                engine.framebuffer_size_callback(engine.window, 1200, 800);
            }

        }
    }

    // Beenden Sie GLFW
    engine.clean();
    glfwTerminate();
}

void Menu::renderSimulation()
{
    //get the DataFolder
    double deltaTime = 1;
    int numParticle = 1;
    int numTimeSteps = 1;

    std::string input;
    std::getline(std::cin, input);

    while (true)
    {
        std::string dataPath = "Data"; // Pfad zum Data-Ordner
        std::cout << endl;
        std::cout << "Available simulations: \n\n";

        // Überprüfen, ob das Verzeichnis existiert und darauf zugegriffen werden kann
        if (fs::exists(dataPath) && fs::is_directory(dataPath)) {
            int i = 1;
            for (const auto& entry : fs::directory_iterator(dataPath)) {
                std::cout << "[" << i << "] " << entry.path().filename() << std::endl;
                i++;
            }
        }
        else {
            std::cout << "No available simulations. The 'Data' folder is missing or inaccessible.\n";
        }


        //check wich folder to use based on the i number from above
        std::string Input;
        std::getline(std::cin, Input);
        if (Input == "")
        {
            break;
        }
        else
        {
            int i = std::stoi(Input);
            int j = 1;
            for (const auto& entry : fs::directory_iterator("Data"))
            {
                if (i == j)
                {
                    dataFolder = entry.path().filename().string();

                    // Get the number of timesteps and the particlesize from the info.txt file, separated by ';'
                    std::string infoFile = "Data/" + dataFolder + "/info.txt";
                    std::ifstream file(infoFile);
                    if (!file.is_open()) {
                        std::cerr << "Error opening file: " << infoFile << std::endl;
                        break;
                    }

                    std::string line;
                    // read the first line
                    if (std::getline(file, line)) {
                        try {
                            numTimeSteps = std::stoi(line);
                        }
                        catch (const std::invalid_argument& e) {
                            std::cerr << "Invalid number format for time steps: " << line << std::endl;
                            file.close();
                            break;
                        }
                    }
                    // read the second line
                    if (std::getline(file, line)) {
                        try {
                            numParticle = std::stoi(line);
                        }
                        catch (const std::invalid_argument& e) {
                            std::cerr << "Invalid number format for particle size: " << line << std::endl;
                            file.close();
                            break;
                        }
                    }
                    //read the third line
                    if (std::getline(file, line)) {
                        try {
                            deltaTime = std::stod(line);
                        }
                        catch (const std::invalid_argument& e) {
                            std::cerr << "Invalid number format for delta time: " << line << std::endl;
                            file.close();
                            break;
                        }
                    }

                    file.close();
                    break;
                }
                j++;
            }
            break;
        }
    }
    Physics* physics = new Physics(dataFolder);
    if (physics->configFile) physics->config();
    physics->deltaTime = deltaTime;
    physics->particlesSize = numParticle;
    physics->numTimeSteps = numTimeSteps;

    //choose a Name for the video
    std::string videoName;
    std::cout << "\nPlease enter a name for the video: ";
    std::getline(std::cin, videoName);
        
    //choose weather to rotate the camara around the center or not
    bool rotate = false;
    std::cout << "\nDo you want to rotate the camera around the center? (y/n): ";
    std::string input2;
    std::getline(std::cin, input2);
    if (input2 == "y")
    {
		rotate = true;
        //choose the speed of the rotation
        double speed;
        std::cout << "\nPlease enter the speed of the rotation(between 10-1000):  ";
        std::getline(std::cin, input2);
        speed = std::stod(input2);
	}

    //Choose one of the following Colormodes
    int colorMode = 0;
    std::cout << "\nPlease choose a color mode: \n";
    std::cout << "[1] Visible ALl\n";
    std::cout << "[2] Visible only Baryonic\n";
    std::cout << "[3] Visible only DarkMatter\n\n";
    std::cout << "[4] Thermal All\n";
    std::cout << "[5] Thermal only Baryonic\n";
    std::cout << "[6] Thermal only DarkMatter\n\n";
    std::cout << "[7] Density All\n";
    std::cout << "[8] Density only Baryonic\n";
    std::cout << "[9] Density only DarkMatter\n\n";

    std::string input3;
    std::getline(std::cin, input3);
    colorMode = std::stoi(input3);


}