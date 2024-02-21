cmake_minimum_required(VERSION 3.10)
project(MeinProjekt)



# Setzen Sie den C++17-Standard
set(CMAKE_CXX_STANDARD 17)

# Stellen Sie sicher, dass Sie die gewünschte Hauptdatei festlegen
set(MAIN_SOURCE "GLApp/GLApp/main.cpp")

# Inkludieren Sie die OpenGl-Bibliothek
find_package(OpenGL REQUIRED)

# Inkludieren und Verknüpfen Sie GLFW
find_package(glfw3 REQUIRED)
include_directories(${CMAKE_SOURCE_DIR}/External_Libraries_Linux/GLFW/include)
link_directories(${CMAKE_SOURCE_DIR}/External_Libraries_Linux/GLFW/lib) # Passen Sie dies an den tatsächlichen Pfad Ihrer GLFW-Bibliotheken an

# Inkludieren und Verknüpfen Sie GLEW
find_package(GLEW REQUIRED)
include_directories(${CMAKE_SOURCE_DIR}/External_Libraries_Linux/GLEW/include)
link_directories(${CMAKE_SOURCE_DIR}/External_Libraries_Linux/GLEW/lib) # Passen Sie dies an den tatsächlichen Pfad Ihrer GLEW-Bibliotheken an

# Inkludieren Sie GLM
include_directories(${CMAKE_SOURCE_DIR}/External_Libraries_Linux/GLM/glm)

# include all cpp and .h files
file(GLOB_RECURSE HEADER_FILES "GLApp/GLApp/*.h")
file(GLOB_RECURSE SOURCE_FILES "GLApp/GLApp/*.cpp")

include_directories(${CMAKE_SOURCE_DIR}/GLApp/GLApp/Physics/)
include_directories(${CMAKE_SOURCE_DIR}/GLApp/GLApp/File/)
include_directories(${CMAKE_SOURCE_DIR}/GLApp/GLApp/Physics/BarnesHut/)
include_directories(${CMAKE_SOURCE_DIR}/GLApp/GLApp/Physics/Galaxies/)
include_directories(${CMAKE_SOURCE_DIR}/GLApp/GLApp/Rendering/)
include_directories(${CMAKE_SOURCE_DIR}/GLApp/GLApp/Math/)

include_directories(${CMAKE_SOURCE_DIR})
add_executable(MeinProgramm ${MAIN_SOURCE} ${HEADER_FILES} ${SOURCE_FILES})

# Linken Sie die erforderlichen Bibliotheken
target_link_libraries(MeinProgramm PRIVATE ${OPENGL_LIBRARIES} glfw GLEW pthread)

# Fügen Sie die Verknüpfung mit ncurses hinzu
find_package(Curses REQUIRED)
target_link_libraries(MeinProgramm PRIVATE ${CURSES_LIBRARIES})
