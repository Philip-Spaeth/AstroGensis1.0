#Welcome to our Software

[![Example Milkyway Andromeda Collision]          // Title
(https://i.ytimg.com/vi/Hc79sDi3f0U/maxresdefault.jpg)] // Thumbnail
(https://www.youtube.com/watch?v=K3MwyhXIuEA "Now in Android: 55")    // Video Link

For instructions on how to use this software open the getting started file under /Doku



Setup Project on Linux:
Cmake installieren:
	sudo apt-get update
	sudo apt-get install cmake g++ # Beispiel für Ubuntu

openGL bibs installieren:
	sudo apt-get install libglm-dev
	sudo apt-get install libglu1-mesa-dev

sudo apt-get install libncurses5-dev

CmakeLists.txt    in main Order:
	cmake_minimum_required(VERSION 3.10)
	project(MeinProjekt)

	add_executable(MeinProgramm main.cpp) # Hier main.cpp durch 	die richtigen Quellcode-Dateien ersetzen


mkdir build
cd build
cmake ..

ausführen:
./MeinProgramm
