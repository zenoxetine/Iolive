#include "Live2DManager.hpp"
#include <CubismModelSettingJson.hpp>

#include "Utility.hpp"
#include <filesystem>
#include <string.h>

bool Live2DManager::InitCubism()
{
	if (!CubismFramework::IsInitialized())
	{
		// Initialize CubismFramework
		s_CubismOption.LoggingLevel = CubismFramework::Option::LogLevel_Verbose;
		s_CubismOption.LogFunction = [](const char* message) { LoggingFunction(message); };
		CubismFramework::StartUp(&s_CubismAllocator, &s_CubismOption);
		CubismFramework::Initialize();
	}

	return CubismFramework::IsInitialized();
}

void Live2DManager::ReleaseCubism()
{
	CubismFramework::Dispose();
}

Model2D* Live2DManager::CreateModel(const wchar_t* modelJson)
{
	// load .model3.json
	auto [buffer, fileSize] = Utility::CreateBufferFromFile(modelJson);
	if (!buffer) return false;
	ICubismModelSetting* modelSetting = new CubismModelSettingJson(reinterpret_cast<csmByte*>(buffer), fileSize);
	delete[] buffer;

	// check model setting (.model3.json)
	bool jsonOK = CheckModelSetting(modelSetting);
	if (!jsonOK)
	{
		delete modelSetting;
		return false;
	}

	// get model absolute dir & filename
	std::filesystem::path modelPath = modelJson;
	std::wstring modelDir = modelPath.parent_path().wstring() + L'/'; // c:\\a\\b/c, it's okay
	std::wstring modelFilename = modelPath.filename().wstring();

	// create new model!
	LoggingFunction("[Live2DManager][I] Creating new model ...\n");
	Model2D* newModel = new Model2D(modelSetting, modelDir.data(), modelFilename.data());
	if (newModel->IsInitialized())
	{
		LoggingFunction("[Live2DManager][I] New Model initialized\n\n");
		return newModel;
	}
	else
	{
		LoggingFunction("[Live2DManager][E] Error while creating new model\n");
		delete newModel;

		return nullptr;
	}
}

bool Live2DManager::CheckModelSetting(ICubismModelSetting* modelSetting)
{
	// check .moc3 file
	const char* mocFilename = modelSetting->GetModelFileName();
	if (strlen(mocFilename) == 0)
	{
		LoggingFunction("[Live2DManager][E] Can't found .moc3 from the json file\n\n");
		return false;
	}

	for (int i = 0; i < strlen(mocFilename); i++)
	{
		if (mocFilename[i] < 0)
		{
			// seems "model filename" contains unicode value, ex: japanese text
			// JsonParser from CubismFramework doesn't support wchar_t and will return an unexpected value
			LoggingFunction("[Live2DManager][E] Can't load model that contains a unicode in the filename\n\n");
			return false;
		}
	}

	// okay!
	return true;
}