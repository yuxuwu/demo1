#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <fstream>
#include <thread>

#define RES_X 800
#define RES_Y 600


const std::string readFromFile(const std::string &pathToFile);
const std::string readFromFile(const std::string &pathToFile) {
	std::string content;
	std::ifstream fileStream(pathToFile, std::ios::in);

	if(!fileStream.is_open()) {
		std::cerr << "Could not read file " << pathToFile << ". File does not exist." << std::endl;
		return "";
	}

	std::string line = "";
	while(!fileStream.eof()) {
		std::getline(fileStream, line);
		content.append(line + "\n");
	}

	fileStream.close();
	return content;
}

void processInput(GLFWwindow *window);
void processInput(GLFWwindow *window) {
	if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
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

	GLFWwindow* window = glfwCreateWindow(RES_X, RES_Y, "LearnOpenGL", NULL, NULL);
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
		-1.0f,  1.0f,  0.0f, //top left corner
		1.0f,  1.0f,  0.0f, //top right corner
		1.0f, -1.0f,  0.0f, //bot right corner
		-1.0f, -1.0f,  0.0f //bot left corner
	};
	unsigned int indicies[] = {
		0, 1, 2,
		0, 2, 3
	};

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
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicies), indicies, GL_STATIC_DRAW);
	// Link Vert Attribs
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// Unbind VAO to prevent future modification
	glBindBuffer(GL_ARRAY_BUFFER, 0); 
	glBindVertexArray(0); 


	/// Shaders
	// Vert Shader
	const char* vertShaderSource = "#version 330 core\n layout (location = 0) in vec3 aPos;\n void main(){ gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);}\0";
	unsigned int vertShader;
	vertShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertShader, 1, &vertShaderSource, NULL);
	glCompileShader(vertShader);
	int success;
	char infoLog[512];
	glGetShaderiv(vertShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	} else {
		std::cout << "VERT SHADER SUCCESS" << std::endl;
	}


	// Frag Shader
	std::string sfragShaderSource = readFromFile("../rm_test.frag");
	const char* fragShaderSource = sfragShaderSource.c_str();
	unsigned int fragShader;
	fragShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragShader, 1, &fragShaderSource, NULL);
	glCompileShader(fragShader);
	glGetShaderiv(fragShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	} else {
		std::cout << "FRAG SHADER SUCCESS" << std::endl;
	}


	// Final Shader
	unsigned int shaderProgram;
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertShader);
	glAttachShader(shaderProgram, fragShader);
	glLinkProgram(shaderProgram);
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	} else {
		std::cout << "SHADER LINKING SUCCESS" << std::endl;
	}

	while(!glfwWindowShouldClose(window)) {
		processInput(window);

		// Clear screen
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		int resolutionLocation = glGetUniformLocation(shaderProgram, "iResolution");
		int timeLocation = glGetUniformLocation(shaderProgram, "iTime");
		// Draw shape
		glUseProgram(shaderProgram);

		glUniform2f(resolutionLocation, RES_X, RES_Y);
		glUniform1f(timeLocation, glfwGetTime());

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteShader(vertShader);
	glDeleteShader(fragShader);
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glfwTerminate();

	return 0;
}
