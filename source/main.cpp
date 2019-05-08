#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <spdlog/spdlog.h>

#include "utils/ShaderProgram.h"

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
		spdlog::error("Failed to create GLFW window");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		spdlog::error("Failed to initialize GLAD");
		return -1;
	}
	glViewport(0, 0, RES_X, RES_Y);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


	/// Shaders
	Shader::ShaderProgram sp("../shaders/basicVert.vert", "../shaders/basicFrag.frag");

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
	// VAO: Vertex Array Object (Defines configuration states for a VBO)
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// VBO: Vertex Buffer Object (How much data we have, and to buffer the data)
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// Define Vertex Attributes (How a vertex is defined in memory)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// EBO: Element Buffer Object (Which vertex index to draw)
	unsigned int EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicies), indicies, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);


	/* LET'S  ROLLLL */
	glClearColor(0.01f, 0.0f, 0.99f, 1.0f);
	sp.use();
	while (!glfwWindowShouldClose(window)) {
		processInput(window);

		glClear(GL_COLOR_BUFFER_BIT);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glfwTerminate();

	return 0;
}
