#ifndef MENU_H
#define MENU_H

#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Particle.h"
#include "Physics.h"
#include "Engine.h"
#include "FileManager.h"

class Menu 
{
public:
	Menu() {};
	~Menu() {};

	void startMenu();

private:
	std::string dataFolder = "";
	std::string dataFile = "";

	void calculateSimulation();
	void viewSimulation();
	void renderSimulation();
};

#endif