#ifndef FONTS_H
#define FONTS_H

#include <freetype2/ft2build.h>
#include FT_FREETYPE_H  

#include <map>

struct Character {
    GLuint     TextureID;  // ID handle of the glyph texture
    glm::ivec2 Size;       // Size of glyph
    glm::ivec2 Bearing;    // Offset from baseline to left/top of glyph
    GLuint     Advance;    // Offset to advance to next glyph
};

std::map<GLchar, Character> Characters;

class Font {
public:
  FT_Face face;
  FT_Library ft;
  Font(const char* fontPath, const FTUInt &pixel_width, const FTUInt &pixel_height ) {
    // 1. Find font
    if (FT_Init_FreeType(&ft))
      std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
    if (FT_New_Face(ft, "../VCR.ttf", 0, &face))
      std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;  

    // 2. Load font
    FT_Set_Pixel_Sizes(face, 0, 48);  
    if (FT_Load_Char(face, 'X', FT_LOAD_RENDER))
      std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;  

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction

    for (GLubyte c = 0; c < 128; c++) {
      // Load character glyph
      if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
	std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
	continue;
      }
      // Generate texture
      GLuint texture;
      glGenTextures(1, &texture);
      glBindTexture(GL_TEXTURE_2D, texture);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width,
		   face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE,
		   face->glyph->bitmap.buffer);
      // Set texture options
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      // Now store character for later use
      Character character = {
	  texture,
	  glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
	  glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
	  face->glyph->advance.x};
      Characters.insert(std::pair<GLchar, Character>(c, character));
    }
  }
}

#endif
