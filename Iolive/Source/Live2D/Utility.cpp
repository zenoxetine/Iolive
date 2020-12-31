#include "Utility.hpp"
#include <sys/stat.h>
#include <fstream>
// #include <sstream>
// #include <codecvt>

namespace Utility {

	std::tuple<unsigned char*, int> CreateBufferFromFile(const wchar_t* filePath)
	{
		int fileSize = 0;

		// known file size
		struct _stat64i32 statBuf;
		if (_wstat64i32(filePath, &statBuf) == 0)
		{
			fileSize = statBuf.st_size;
		}

		std::basic_fstream<unsigned char> file;
		unsigned char* buf = new unsigned char[fileSize];

		file.open(filePath, std::ios::in | std::ios::binary);
		if (!file.is_open())
		{
			// error while opening the file
			return { nullptr, fileSize };
		}

		file.read(buf, fileSize);
		file.close();

		return { buf, fileSize };
	}

	wchar_t* NewWideChar(const char* value)
	{
		const size_t charSize = strlen(value) + 1;
		wchar_t* wValue = new wchar_t[charSize];
		std::mbstowcs(wValue, value , charSize);
		
		return wValue;
	}

	/*std::tuple<wchar_t*, int> WReadFile(const wchar_t* filePath)
	{
		int fileSize = 0;

		struct _stat64i32 statBuf;
		if (_wstat64i32(filePath, &statBuf) == 0)
		{
			fileSize = statBuf.st_size;
		}

		std::wifstream wif(filePath);
		wif.imbue(std::locale(std::locale::empty(), new std::codecvt_utf8<wchar_t>));
		std::wstringstream wss;
		wss << wif.rdbuf();
		return { _wcsdup(wss.str().c_str()), fileSize};
	}*/
}