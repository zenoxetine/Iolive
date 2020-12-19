#pragma once

#include <tuple>

namespace Utility {

	std::tuple<unsigned char*, int> CreateBufferFromFile(const wchar_t* filePath);

	// create new heap allocated wide char
	// don't forget to delete[] it
	wchar_t* NewWideChar(const char* value);

	// std::tuple<wchar_t*, int> WReadFile(const wchar_t* filename);
}