#include "TextureManager.hpp"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_STDIO
#define STBI_ONLY_PNG
#include "stb_image.h"

#include "../Utility.hpp"

TextureManager::TextureManager()
{}

TextureManager::~TextureManager()
{
	for (GLuint& texId : texturesId)
	{
		glDeleteTextures(1, &(texId));
	}
}

bool TextureManager::CreateTextureFromPngFile(const wchar_t* filePath)
{
	auto[buffer, bufSize] = Utility::CreateBufferFromFile(filePath);
	if (!buffer) return false;

	int width, height, channels;

	// load image from buffer
	unsigned char* png = stbi_load_from_memory(
		buffer,
		bufSize,
		&width,
		&height,
		&channels,
		STBI_rgb_alpha
	);

	GLuint textureId; // texture id

	// prepare & binding
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);

	// set texture option/filter
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// generate the texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, png);
	glGenerateMipmap(GL_TEXTURE_2D);

	// unbind texture
	glBindTexture(GL_TEXTURE_2D, 0);

	stbi_image_free(png);
	delete[] buffer;

	texturesId.push_back(textureId);

	return true;
}

GLuint TextureManager::GetTextureAt(const int index)
{
	return texturesId.at(index);
}