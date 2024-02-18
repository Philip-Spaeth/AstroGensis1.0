#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <cmath>
#include "Particle.h"

//  Nur unter Windows
#include "Engine.h"
#ifdef WIN32
#include <Windows.h>
#include <debugapi.h>
#endif
/*
#else
#include <ncurses.h>
#include <stdio.h>
*/
#include "Menu.h"

//#ifdef WIN32
int main()
{
    Menu menu;
    menu.startMenu();

    return 0;
}