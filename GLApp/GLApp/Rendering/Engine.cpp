#include "Engine.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <gtc/matrix_transform.hpp>
#include <Windows.h>
#include <gtc/type_ptr.hpp>
#include <chrono>
#include <thread>
#include <string>

 Engine::Engine() : window(nullptr), shaderProgram(0), VAO(0)
{
     // start kamera position
     cameraPosition = glm::vec3(0.0f, 0.0f, 1000.0f);                     
     cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
     cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
     cameraYaw = -90.0f;
     cameraPitch = 0.0f;
}

bool Engine::init(double physicsFaktor) 
{
    faktor = physicsFaktor;
    // GLFW initialisieren
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }

    // GLFW-Konfiguration setzen
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    // Ein GLFW-Fenster erstellen
    window = glfwCreateWindow(1200, 800, "Particle Rendering", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }

    // GLFW-Kontext setzen
    glfwMakeContextCurrent(window);

    // GLEW initialisieren
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return false;
    }

    // Shader-Programm kompilieren und verlinken
    const char* vertexShaderSource = "#version 330 core\n"
        "layout (location = 0) in vec3 position;\n"
        "uniform mat4 projection;\n"
        "uniform mat4 view;\n"
        "uniform vec3 particlePosition; // Neue Uniform-Variable f�r die Partikelposition\n"
        "void main()\n"
        "{\n"
        "    // Berechnen Sie die endg�ltige Position des Partikels, indem Sie die Partikelposition hinzuf�gen\n"
        "    vec4 finalPosition = projection * view * vec4(position + particlePosition, 1.0);\n"
        "    gl_Position = finalPosition;\n"
        "}\0";

    const char* fragmentShaderSource = "#version 330 core\n"
        "out vec4 FragColor;\n"
        "uniform vec3 particleColor;\n"
        "void main()\n"
        "{\n"
        "    FragColor = vec4(particleColor, 1.0); // Wei�\n"
        "}\n\0";

    // Erstellen des Shader-Programms und kompilieren
    GLuint vertexShader, fragmentShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    checkShaderCompileStatus(vertexShader, "VERTEX");

    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    checkShaderCompileStatus(fragmentShader, "FRAGMENT");

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    checkShaderLinkStatus(shaderProgram);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    return true;
}

void Engine::start()
{
    // Erstellen des FileManagers
    fileManager = new FileManager();
    // Laden der Daten für die Darstellung
    std::vector<Particle> particles;
    fileManager->loadParticles(0,particles);

    // Hier VBO und VAO erstellen und konfigurieren
    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * particles.size(), &particles[0], GL_STATIC_DRAW);

    // Erstellen des Vertex Array Objects (VAO)
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Konfigurieren des VAO f�r das VBO
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //place the BGstars in the background
    if (BGstars)
    {
        for (int i = 0; i < amountOfStars; i++)
        {
            double x = random(-1e14, 1e14);
			double y = random(-1e14, 1e14);
			double z = random(-1e14, 1e14);
            double size = random(0.1, 2);
			stars.push_back(glm::vec4(x, y, z, size));
		}   
    }


    std::cout << "Data loaded" << std::endl;
}

void Engine::update(int index, std::vector<Particle>& particles)
{
    //calculate the time
    if (isRunning) 
    {
        if (particles.size() >= 4) {
            calcTime(particles[4].position, index);
        }
        else {
            calcTime(index);
        }
    }

    processMouseInput();
    processInput();

    // set the globalScale of the system
    if (index == 0) {
        calculateGlobalScale(particles);
    }

    renderParticles(particles);

    glfwSwapBuffers(window);
    glfwPollEvents();

    //pause if space is pressed
    if (GetAsyncKeyState(32) & 0x8000)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        isRunning = !isRunning;
    }

    //speed up if right arrow is pressed
    if (GetAsyncKeyState(39) & 0x8000)
    {
		playSpeed = playSpeed + changeSpeed;
	}
    //slow down if left arrow is pressed
    if (GetAsyncKeyState(37) & 0x8000)
    {
        playSpeed = playSpeed - changeSpeed;
    }

    //if 1 is pressed
    if (GetAsyncKeyState(49) & 0x8000)
    {
		//set the play speed to 1
		playSpeed = 1;
	}

    //disable / enable dark matter with Z
    if (GetAsyncKeyState(90) & 0x8000)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
		showDarkMatter = !showDarkMatter;
	}

}

void Engine::renderParticles(std::vector<Particle>& particles)
{
    // L�schen des Bildschirms
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Deaktivieren Sie den Tiefentest und das Z-Buffering
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    // In der renderParticles-Funktion
    glUseProgram(shaderProgram);

    // Erstellen der Projektionsmatrix und far !!!!!!!
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, (float)cameraViewDistance);

    // Setzen der Matrizen im Shader
    GLuint projectionLoc = glGetUniformLocation(shaderProgram, "projection");
    GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");

    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    // Vertex Array Object (VAO) binden
    glBindVertexArray(VAO);

    if (BGstars)
    {
        //render the background stars
        for (int i = 0; i < amountOfStars; i++)
        {
            glPointSize(stars[i].w);

            // Setzen Sie die Position im Shader
            glm::vec3 pos = glm::vec3(stars[i].x, stars[i].y, stars[i].z);
            glUniform3fv(glGetUniformLocation(shaderProgram, "particlePosition"), 1, glm::value_ptr(pos));

            // Setzen Sie die Farbe im Shader
            glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
            glUniform3fv(glGetUniformLocation(shaderProgram, "particleColor"), 1, glm::value_ptr(color));

            // Zeichnen Sie den Punkt
            glDrawArrays(GL_POINTS, 0, 1);
        }
    }
    if (tracks == false)
    {
        for (int p = 0; p < particles.size(); p++)
        {
            if(particles[p].radius == 0)
			{
				continue;
			}
            if(showDarkMatter == false && particles[p].darkMatter == true)
			{
                continue;
			}
            if (showDarkMatter == true && particles[p].darkMatter == true)
            {
                // Setzen Sie die Farbe auf blau für dunkle Materie
                glUniform3fv(glGetUniformLocation(shaderProgram, "particleColor"), 1, glm::value_ptr(glm::vec3(0,0,1)));
            }
            else
			{
				// Setzen Sie die Farbe im Shader
				glUniform3fv(glGetUniformLocation(shaderProgram, "particleColor"), 1, glm::value_ptr(particles[p].color));
			}

            glm::vec3 scaledPosition = glm::vec3(
                particles[p].position.x * globalScale,
                particles[p].position.y * globalScale,
                particles[p].position.z * globalScale
            ); 

            // Setzen Sie die Position im Shader
            glUniform3fv(glGetUniformLocation(shaderProgram, "particlePosition"), 1, glm::value_ptr(scaledPosition));

            glPointSize(particles[p].radius);

            // Zeichnen Sie den Punkt
            glDrawArrays(GL_POINTS, 0, 1);
        }
    }

    // VAO l�sen
    glBindVertexArray(0);
}
void Engine::processInput()
{
    if (GetAsyncKeyState(VK_CONTROL) < 0)
    {
        // Kamerabewegung
        float index = 0.1f; // �ndern Sie diesen Wert je nach Bedarf
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            cameraPosition += rushSpeed * index * cameraFront;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            cameraPosition -= rushSpeed * index * cameraFront;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            cameraPosition -= rushSpeed * index * glm::normalize(glm::cross(cameraFront, cameraUp));
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            cameraPosition += rushSpeed * index * glm::normalize(glm::cross(cameraFront, cameraUp));
    }
    else
    {
        // Kamerabewegung
        float index = 0.1f; // �ndern Sie diesen Wert je nach Bedarf
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            cameraPosition += cameraSpeed * index * cameraFront;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            cameraPosition -= cameraSpeed * index * cameraFront;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            cameraPosition -= cameraSpeed * index * glm::normalize(glm::cross(cameraFront, cameraUp));
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            cameraPosition += cameraSpeed * index * glm::normalize(glm::cross(cameraFront, cameraUp));
    }
    // Aktualisieren der Ansichtsmatrix (Kameraposition und Blickrichtung)
    view = glm::lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp);

    // Setzen der Matrizen im Shader
    GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
}

// F�gen Sie diese Methode zur Engine-Klasse hinzu
void Engine::processMouseInput()
{

    // Erfassen Sie die Mausbewegung
    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);

    static double lastMouseX = mouseX;
    static double lastMouseY = mouseY;

    double xOffset = mouseX - lastMouseX;
    double yOffset = lastMouseY - mouseY; // Umgekehrtes Vorzeichen f�r umgekehrte Mausrichtung

    lastMouseX = mouseX;
    lastMouseY = mouseY;

    const float sensitivity = 0.05f;
    xOffset *= sensitivity;
    yOffset *= sensitivity;

    cameraYaw += xOffset;
    cameraPitch += yOffset;

    // Begrenzen Sie die Kamerapitch, um ein �berdrehen zu verhindern
    if (cameraPitch > 89.0f)
        cameraPitch = 89.0f;
    if (cameraPitch < -89.0f)
        cameraPitch = -89.0f;

    // Berechnen der neuen Kamerarichtung
    glm::vec3 newFront;
    newFront.x = cos(glm::radians(cameraYaw)) * cos(glm::radians(cameraPitch));
    newFront.y = sin(glm::radians(cameraPitch));
    newFront.z = sin(glm::radians(cameraYaw)) * cos(glm::radians(cameraPitch));
    cameraFront = glm::normalize(newFront);

    // Aktualisieren der Ansichtsmatrix
    view = glm::lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp);
}

void Engine::checkShaderCompileStatus(GLuint shader, const char* shaderType)
{
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cerr << "Error compiling " << shaderType << " shader:\n" << infoLog << std::endl;
    }
}

void Engine::checkShaderLinkStatus(GLuint program)
{
    GLint success;
    GLchar infoLog[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);

    if (!success) {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cerr << "Error linking shader program:\n" << infoLog << std::endl;
    }
}
bool Engine::clean()
{
    // Aufr�umen und beenden
    glfwTerminate();
    return true;
}

void Engine::calcTime(int index) {
    calcTime(glm::dvec3(0, 0, 0), index);
}

void Engine::calcTime(glm::dvec3 position, int index)
{
    passedTime = (index * faktor);

    int passedTimeInSec = passedTime / 86400;

    std::string Unit;

    //set the right unit
    if (passedTime < 60) { Unit = " s"; }
    else if (passedTime < 3600) { passedTime /= 60; Unit = " min"; }
    else if (passedTime < 86400) { passedTime /= 3600; Unit = " h"; }
    else if (passedTime < 31536000) { passedTime /= 86400; Unit = " days"; }
    else { passedTime /= 31536000; Unit = " years"; }

    // Berechne das Enddatum basierend auf der "passedTime"
    int startYear = 2020;
    int startMonth = 01;
    int startDay = 01;

    // Berechnen Sie das aktuellen datums basierend auf der "passedTimeInSec"
    int currentYear = startYear;
    int currentMonth = startMonth;
    int currentDay = startDay;

    // Berechnen Sie das aktuelle Datum basierend auf der "passedTimeInSec"
    while (passedTimeInSec > 0) {
        // Berechnen Sie die Anzahl der Tage im aktuellen Monat
        int daysInMonth = 0;
        switch (currentMonth) {
        case 1: daysInMonth = 31; break;
        case 2: daysInMonth = 28; break;
        case 3: daysInMonth = 31; break;
        case 4: daysInMonth = 30; break;
        case 5: daysInMonth = 31; break;
        case 6: daysInMonth = 30; break;
        case 7: daysInMonth = 31; break;
        case 8: daysInMonth = 31; break;
        case 9: daysInMonth = 30; break;
        case 10: daysInMonth = 31; break;
        case 11: daysInMonth = 30; break;
        case 12: daysInMonth = 31; break;
        }

        // �berpr�fen Sie, ob das aktuelle Jahr ein Schaltjahr ist
        bool isLeapYear = false;
        if (currentYear % 4 == 0) {
            if (currentYear % 100 == 0) {
                if (currentYear % 400 == 0) {
                    isLeapYear = true;
                }
            }
            else {
                isLeapYear = true;
            }
        }

        // �berpr�fen Sie, ob das aktuelle Jahr ein Schaltjahr ist
        if (isLeapYear && currentMonth == 2) {
            daysInMonth = 29;
        }

        // �berpr�fen Sie, ob das aktuelle Jahr ein Schaltjahr ist
        if (currentDay == daysInMonth) {
            currentDay = 1;
            if (currentMonth == 12) {
                currentMonth = 1;
                currentYear++;
            }
            else {
                currentMonth++;
            }
        }
        else {
            currentDay++;
        }

        passedTimeInSec--;
    }

    
    std::string day = std::to_string(currentDay);
    if (currentDay < 10)
    {
        day = "0" + day;
    }
    std::string month = std::to_string(currentMonth);
    if (currentMonth < 10)
    {
		month = "0" + month;
	}
    
    if (month == "01")
    {
        if (day == "01")
        {
            std::cout << position.x << " " << position.y << " " << position.z << std::endl;
		}
    }
    
    if (passedTime < 1000000)
    {
        std::cout << "passed time: " << (int)passedTime << Unit << "    date: " << currentYear << "." << month << "." << day << std::endl;
    }
    else
    {
        //print out the past time in the right unit
        std::cout << std::scientific << std::setprecision(0) << "passed time: " << passedTime << Unit << "    date: " << currentYear << "." << month << "." << day << std::endl;
    }
}
double Engine::random(double min, double max)
{
    // Generiere eine zufällige Gleitkommazahl zwischen min und max
    double randomFloat = min + static_cast<double>(rand()) / static_cast<double>(RAND_MAX) * (max - min);

    return randomFloat;
}

void Engine::calculateGlobalScale(std::vector<Particle>& particles) {
    double maxDistance = 0;
    for (int i = 0; i < particles.size(); i++){
	    double distance = sqrt(pow(particles[i].position.x, 2) + pow(particles[i].position.y, 2) + pow(particles[i].position.z, 2));
        if (distance > maxDistance){
		    maxDistance = distance;
	    }
	}

    // Distance is the diameter of the system
    maxDistance = maxDistance * 2;

    // Exponenten finden
    int exponent;

    if (maxDistance != 0) {
        exponent = static_cast<int>(std::floor(std::log10(std::abs(maxDistance))));
    }
    else {
        exponent = 0; // Der Logarithmus von 0 ist nicht definiert.
    }

	globalScale = maxDistance / pow(10, exponent*2 - 2);
    //globalScale = 1e-18;
}
