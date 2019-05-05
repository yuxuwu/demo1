#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <cmath>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <thread>

#define RES_X 1252
#define RES_Y 704

void processInput(GLFWwindow *window);
void processInput(GLFWwindow *window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
	glViewport(0, 0, width, height);
}

int main() {
	/*
	 * Window Context setup
	 */
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow *window = glfwCreateWindow(RES_X, RES_Y, "Demo1", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	glViewport(0, 0, RES_X, RES_Y);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	/*
	 * Sample scene setup
	 */
	// Covers screen
	float vertices[] = {
						// Upper Tri
						-1.0f, 1.0f,  0.0f, // top left corner
						1.0f,  1.0f,  0.0f, // top right corner
						1.0f,  -1.0f, 0.0f, // bot right corner
						-1.0f, -1.0f, 0.0f  // bot left corner
	};
	unsigned int indicies[] = {0, 1, 2, 0, 2, 3};

	/// Buffers
	// Gen. Buffers
	unsigned int VBO, VAO, EBO;
	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &EBO);
	// Bind buffers, VAO first
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicies), indicies,
				 GL_STATIC_DRAW);
	// Link Vert Attribs
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);
	// Unbind VAO to prevent future modification
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	/// Shaders

	/* LET'S  ROLLLL */
	while (!glfwWindowShouldClose(window)) {
		processInput(window);

		glClearColor(0.01f, 0.0f, 0.99f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glfwTerminate();

	return 0;
}
