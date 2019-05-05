cmake_minimum_required (VERSION 3.1)
project (3d-test)


## Source files
set (SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/source/") # my source files
set (SOURCES "${SRC_DIR}/main.cpp")
set (VEN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/vendors/") # submodules


## Executable definition
add_executable (${PROJECT_NAME} ${SOURCES})
target_include_directories (${PROJECT_NAME} PRIVATE ${INC_DIR})
set_property (TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 11)


## GLFW
set (GLFW_DIR "${VEN_DIR}/glfw/")
# Don't want to build these
set (GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "Build the GLFW example programs")
set (GLFW_BUILD_TESTS OFF CACHE INTERNAL "Build the GLFW test programs")
set (GLFW_BUILD_DOCS OFF CACHE INTERNAL "Build the GLFW documentation")
set (GLFW_INSTALL OFF CACHE INTERNAL "Generate installation target")
add_subdirectory ("${GLFW_DIR}")

target_link_libraries (${PROJECT_NAME} "glfw")
target_include_directories (${PROJECT_NAME} PRIVATE "${GLFW_DIR}/include/")
# Don't use gl includes, we supply our own
#target_compile_definitions (${PROJECT_NAME} PRIVATE "GLFW_INCLUDE_NONE")


## glad
set (GLAD_DIR "${VEN_DIR}/glad/")
add_library("glad" "${GLAD_DIR}/src/glad.c")
target_include_directories (${PROJECT_NAME} PRIVATE "${GLAD_DIR}/include/")
target_include_directories ("glad" PRIVATE "${GLAD_DIR}/include/")
target_link_libraries (${PROJECT_NAME} "glad" "${CMAKE_DL_LIBS}")
