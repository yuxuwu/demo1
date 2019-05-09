#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include <spdlog/spdlog.h>

#include <glad/glad.h>
#include <glm.hpp>
#include <gtc/type_ptr.hpp>

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

		void setBool(const std::string &, bool) const;
		void setInt(const std::string &, int) const;
		void setFloat(const std::string &, float) const;
		void setFloat4(const std::string &, float, float, float, float) const;
		void setMat4(const std::string &, glm::mat4) const;
	};
}

#endif
