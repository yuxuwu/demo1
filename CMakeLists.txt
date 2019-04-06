project(3d-test)
cmake_minimum_required(VERSION 2.8)

find_package(OpenGL REQUIRED)
add_subdirectory(vendors/glfw/)

# Header files (.h/.hpp)
include_directories(include/)

# Source files (.c/.cpp)
add_executable( ${PROJECT_NAME}
	main.cpp
	glad.c)

# Library files (.a/.so, we have no .so in this project)
target_link_libraries( ${PROJECT_NAME}
	glfw
	OpenGL::GL)
