#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include <spdlog/spdlog.h>

#include <glad/glad.h>

#include <string>
#include <sstream>
#include <fstream>

namespace Shader {

	std::string getShaderSource(const char* path) {
		std::stringstream sourceStream;
		std::ifstream f;
		// Ensure in stream can throw exceptions
		f.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try {
			// Open file
			f.open(path);
			// Read contents to source
			sourceStream << f.rdbuf();
			// Convert stream to string
			f.close();
			std::string tmpSource = sourceStream.str();

			return tmpSource;

		} catch(std::ifstream::failure e) {
			spdlog::error ("SHADER::File not successfully read.");
			return "";
		}
	};

	void checkCompileErrors(unsigned int shaderID, std::string type) {
		int success;
		char infoLog[1024];

		if (type != "PROGRAM") {
			glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
			if (!success) {
				glGetShaderInfoLog(shaderID, 1024, NULL, infoLog);
				std::ostringstream stream;
				stream << "SHADER::Compilation error of type: " << type << "\n" << infoLog;
				spdlog::error(stream.str());
			}
		} else {
			glGetProgramiv(shaderID, GL_LINK_STATUS, &success);
			if (!success) {
				glGetProgramInfoLog(shaderID, 1024, NULL, infoLog);
				std::ostringstream stream;
				stream << "PROGRAM: Linking error of type: " << type << "\n" << infoLog;
				spdlog::error(stream.str());
			}
		}
	}

	class ShaderProgram {
	private:
		unsigned int m_ID;
	public:
		ShaderProgram(const char* vertShaderPath, const char* fragShaderPath) {
			// Vertex Shader
			std::string v = getShaderSource(vertShaderPath);
			const char* vertSource = v.c_str();
			unsigned int vertID = glCreateShader(GL_VERTEX_SHADER);
			glShaderSource(vertID, 1, &vertSource, NULL);
			glCompileShader(vertID);
			checkCompileErrors(vertID, "VERTEX");

			// Fragment Shader
			std::string f = getShaderSource(fragShaderPath);
			const char* fragSource = f.c_str();
			unsigned int fragID = glCreateShader(GL_FRAGMENT_SHADER);
			glShaderSource(fragID, 1, &fragSource, NULL);
			glCompileShader(fragID);
			checkCompileErrors(fragID, "FRAGMENT");

			/// Program
			m_ID = glCreateProgram();
			glAttachShader(m_ID, vertID);
			glAttachShader(m_ID, fragID);
			glLinkProgram(m_ID);
			checkCompileErrors(m_ID, "PROGRAM");

			// Remove shaders, since they're linked to program now
			glDeleteShader(vertID);
			glDeleteShader(fragID);
		}

		void use(){ glUseProgram(m_ID); };
	};
}

#endif
