#include "Texture.h"

Texture::Texture(std::string texturePath) {
	// Generate texture object
	glGenTextures(1, &m_texID);
	glBindTexture(GL_TEXTURE_2D, m_texID);
	// Texture Wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// Texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Get data from texture path
	int width, height, nrChannels;
	unsigned char* data = stbi_load(texturePath.c_str(), &width, &height, &nrChannels, 0);

	if (data) {
		// Bind Texture
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	} else {
		spdlog::error("Failed to load texture at " + texturePath);
	}

	// Free image
	stbi_image_free(data);
}

unsigned int Texture::id() { return m_texID; }
