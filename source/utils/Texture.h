#ifndef TEXTURE_H
#define TEXTURE_H


#include <spdlog/spdlog.h>
#include <glad/glad.h>
#include <string>

class Texture {
private:
	unsigned int m_texID;

public:
	Texture(std::string);
	unsigned int id();
};

#endif
