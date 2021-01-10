#pragma once

#include <fstream>
#include <cstdlib>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/ostreamwrapper.h>

#include <sys/stat.h>
#include <string>
#include <optional>

inline bool IsFileExist(const wchar_t* filePath)
{
    struct _stat64i32 statBuf;
    return (_wstat64i32(filePath, &statBuf) == 0);
}

class JsonManager
{
public:
    JsonManager()
    {
        document.SetObject();
    }

	bool ReadJson(const wchar_t* filePath)
	{
        if (!IsFileExist(filePath)) return false;

        std::wifstream ifs(filePath);
        if (!ifs.is_open())
        {
            return false;
        }

        rapidjson::WIStreamWrapper isw{ ifs };
        
        document.SetObject(); // reset
        document.ParseStream(isw);

        if (document.HasParseError())
        {
            return false;
        }

        return true;
	}

    void SaveJson(const wchar_t* outFile)
    {
        std::wofstream ofs{ outFile };
        if (!ofs.is_open())
        {
            return;
        }

        rapidjson::WOStreamWrapper osw{ ofs };
        rapidjson::Writer<rapidjson::WOStreamWrapper> writerOut{ osw };
        document.Accept(writerOut);
    }

    static void CreateNewJsonFile(const wchar_t* outFileName, const rapidjson::Document& doc)
    {
        std::wofstream ofs{ outFileName };
        rapidjson::WOStreamWrapper osw{ ofs };
        rapidjson::Writer<rapidjson::WOStreamWrapper> writerOut{ osw };

        doc.Accept(writerOut);
    }

    rapidjson::Document document;
    rapidjson::StringBuffer buffer;
};