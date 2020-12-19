#pragma once

#include <GL/glew.h>
#include <vector>

class TextureManager
{
public:
	TextureManager();

	~TextureManager();

	bool CreateTextureFromPngFile(const wchar_t* filePath);

	GLuint GetTextureAt(const int index);

private:
	std::vector<GLuint> texturesId;
};