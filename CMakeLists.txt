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