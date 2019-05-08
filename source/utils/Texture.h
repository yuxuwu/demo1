#ifndef TEXTURE_H
#define TEXTURE_H

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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
