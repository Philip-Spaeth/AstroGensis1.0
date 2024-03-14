# Inkludieren Sie die OpenGl-Bibliothek
find_package(OpenGL REQUIRED)


if(WIN32)
    # Inkludieren Sie GLM
    include_directories(${CMAKE_SOURCE_DIR}/External_Libraries/GLM/glm)
    # Inkludieren und Verknüpfen Sie GLEW
    find_package(GLEW REQUIRED)
    include_directories(${CMAKE_SOURCE_DIR}/External_Libraries_Linux/GLEW/include)
    link_directories(${CMAKE_SOURCE_DIR}/External_Libraries_Linux/GLEW/lib)
    # Inkludieren und Verknüpfen Sie GLFW
    find_package(glfw3 REQUIRED)
    include_directories(${CMAKE_SOURCE_DIR}/External_Libraries/GLFW/include)
    link_directories(${CMAKE_SOURCE_DIR}/External_Libraries/GLFW/lib) # Passen Sie dies an den tatsächlichen Pfad Ihrer GLFW-Bibliotheken an

elseif(UNIX AND NOT APPLE)
    # Inkludieren Sie GLM
    include_directories(${CMAKE_SOURCE_DIR}/External_Libraries_Linux/GLM/glm)
    # Inkludieren und Verknüpfen Sie GLEW
    find_package(GLEW REQUIRED)
    include_directories(${CMAKE_SOURCE_DIR}/External_Libraries_Linux/GLEW/include)
    link_directories(${CMAKE_SOURCE_DIR}/External_Libraries_Linux/GLEW/lib)
    # Inkludieren und Verknüpfen Sie GLFW
    find_package(glfw3 REQUIRED)
    include_directories(${CMAKE_SOURCE_DIR}/External_Libraries_Linux/GLFW/include)
    link_directories(${CMAKE_SOURCE_DIR}/External_Libraries_Linux/GLFW/lib) # Passen Sie dies an den tatsächlichen Pfad Ihrer GLFW-Bibliotheken an
endif()

# include all cpp and .h files
file(GLOB_RECURSE HEADER_FILES "GLApp/GLApp/*.h")