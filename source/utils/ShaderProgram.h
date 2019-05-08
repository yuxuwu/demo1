#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include <spdlog/spdlog.h>

#include <glad/glad.h>

#include <string>
#include <sstream>
#include <fstream>

namespace Shader {

	std::string getShaderSource(const char*);
	void checkCompileErrors(unsigned int, std::string);

	class ShaderProgram {
	private:
		unsigned int m_ID;
	public:
		ShaderProgram(const char*, const char*);

		unsigned int getID();
		void use();

		void setBool(const std::string &name, bool value) const;
		void setInt(const std::string &name, int value) const;
		void setFloat(const std::string &name, float value) const;
		void setFloat(const std::string &name, float v1, float v2, float v3, float v4) const;

	};
}

#endif
