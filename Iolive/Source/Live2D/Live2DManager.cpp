#include "Live2DManager.hpp"
#include "Utility.hpp"
#include <filesystem>
#include <string.h>

bool Live2DManager::Init()
{
	if (!CubismFramework::IsInitialized())
	{
		// Initialize CubismFramework
		s_CubismOption.LoggingLevel = CubismFramework::Option::LogLevel_Warning;
		s_CubismOption.LogFunction = [](const char* message) { std::printf("%s", message); };
		CubismFramework::StartUp(&s_CubismAllocator, &s_CubismOption);
		CubismFramework::Initialize();
	}

	return CubismFramework::IsInitialized();
}

void Live2DManager::Release()
{
	TryDeleteModel();
	CubismFramework::Dispose();
}

bool Live2DManager::SetModel(const wchar_t* modelJson)
{
	// release previous model
	TryDeleteModel();

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
	s_Model2D = new Model2D(modelSetting, modelDir.data(), modelFilename.data());
	if (s_Model2D->IsInitialized())
	{
		SetupIndexOfDefaultParameter();
		s_IsModelChanged = true; // signal
		return true;
	}
	else
	{
		// error while creating new model
		TryDeleteModel();
		return false;
	}
}

bool Live2DManager::CheckModelSetting(ICubismModelSetting* modelSetting)
{
	// check .moc3 file
	const char* mocFilename = modelSetting->GetModelFileName();
	if (strlen(mocFilename) == 0)
	{
		// can't found .moc3 file from json
		// please use the correct live2d model json!
		return false;
	}

	// can't load model that contains a unicode in the filename
	for (int i = 0; i < strlen(mocFilename); i++)
	{
		if (mocFilename[i] < 0)
		{
			// seems "model filename" contains unicode value, ex: japanese text
			// JsonParser from CubismFramework doesn't support wchar_t and will return an unexpected value
			return false;
		}
	}

	// okay!
	return true;
}

void Live2DManager::SetupIndexOfDefaultParameter()
{
	const char** paramIds = s_Model2D->GetModel()->GetParameterIds();
	uint32_t paramCount = static_cast<uint32_t>(s_Model2D->GetModel()->GetParameterCount());

	for (uint32_t paramIndex = 0; paramIndex < paramCount; paramIndex++)
	{
		if (strcmp(paramIds[paramIndex], "PARAM_ANGLE_X") == 0 ||
			strcmp(paramIds[paramIndex], "ParamAngleX") == 0)
			IndexOfDefaultParameter.ParamAngleX = paramIndex;

		else if(strcmp(paramIds[paramIndex], "PARAM_ANGLE_Y") == 0 ||
				strcmp(paramIds[paramIndex], "ParamAngleY") == 0)
			IndexOfDefaultParameter.ParamAngleY = paramIndex;

		else if (strcmp(paramIds[paramIndex], "PARAM_ANGLE_Z") == 0 ||
				 strcmp(paramIds[paramIndex], "ParamAngleZ") == 0)
			IndexOfDefaultParameter.ParamAngleZ = paramIndex;

		else if (strcmp(paramIds[paramIndex], "PARAM_EYE_L_OPEN") == 0 ||
			strcmp(paramIds[paramIndex], "ParamEyeLOpen") == 0)
			IndexOfDefaultParameter.ParamEyeLOpen = paramIndex;

		else if (strcmp(paramIds[paramIndex], "PARAM_EYE_R_OPEN") == 0 ||
			strcmp(paramIds[paramIndex], "ParamEyeROpen") == 0)
			IndexOfDefaultParameter.ParamEyeROpen = paramIndex;

		else if (strcmp(paramIds[paramIndex], "PARAM_MOUTH_OPEN_Y") == 0 ||
			strcmp(paramIds[paramIndex], "ParamMouthOpenY") == 0)
			IndexOfDefaultParameter.ParamMouthOpenY = paramIndex;
		
		else if (strcmp(paramIds[paramIndex], "PARAM_MOUTH_FORM") == 0 ||
			strcmp(paramIds[paramIndex], "ParamMouthForm") == 0)
			IndexOfDefaultParameter.ParamMouthForm = paramIndex;
		
		else if (strcmp(paramIds[paramIndex], "PARAM_BROW_L_Y") == 0 ||
			strcmp(paramIds[paramIndex], "ParamBrowLY") == 0)
			IndexOfDefaultParameter.ParamBrowLY = paramIndex;
		
		else if (strcmp(paramIds[paramIndex], "PARAM_BROW_R_Y") == 0 ||
			strcmp(paramIds[paramIndex], "ParamBrowRY") == 0)
			IndexOfDefaultParameter.ParamBrowRY = paramIndex;
	}
}

bool Live2DManager::IsModelInitialized()
{
	if (s_Model2D)
		if (s_Model2D->IsInitialized())
			return true;

	return false;
}

void Live2DManager::TryDeleteModel()
{
	if (s_Model2D)
	{
		delete s_Model2D;
		s_Model2D = nullptr;
		ReleaseAllParameterBinding();
		IndexOfDefaultParameter = {};
	}
}

void Live2DManager::OnUpdate(float deltaTime)
{
	s_IsModelChanged = false;

	if (IsModelInitialized())
	{
		// update binded parameter
		for (auto [index, ptrValue] : s_ParameterBinding)
		{
			if (ptrValue)
			{
				s_Model2D->GetModel()->SetParameterValue(index, *ptrValue);
			}
		}

		s_Model2D->GetModel()->SaveParameters();

		s_Model2D->OnUpdate(deltaTime);
	}
}

void Live2DManager::OnDraw(int width, int height)
{
	if (IsModelInitialized())
	{
		s_Model2D->OnDraw(width, height, s_ModelScale);
	}
}

void Live2DManager::SetParameterBinding(int index, float* ptrValue)
{
	s_ParameterBinding[index] = ptrValue;
}

void Live2DManager::ReleaseAllParameterBinding()
{
	s_ParameterBinding.clear();
}

std::map<const char*, float> Live2DManager::GetParameterMap()
{
	const char** paramIds = s_Model2D->GetModel()->GetParameterIds();
	uint32_t paramCount = static_cast<uint32_t>(s_Model2D->GetModel()->GetParameterCount());

	std::map<const char*, float> parameterMap;

	for (uint32_t i = 0; i < paramCount; i++)
	{
		parameterMap.insert(std::pair<const char*, float>(paramIds[i], s_Model2D->GetModel()->GetParameterValue(i)));
	}

	return parameterMap;
}

std::vector<std::array<float, 2>> Live2DManager::GetParameterMinMax()
{
	uint32_t paramCount = static_cast<uint32_t>(s_Model2D->GetModel()->GetParameterCount());

	std::vector<std::array<float, 2>> paramMinMax;
	paramMinMax.reserve(paramCount);

	for (uint32_t i = 0; i < paramCount; i++)
	{
		paramMinMax.push_back({
			s_Model2D->GetModel()->GetParameterMinimumValue(i),
			s_Model2D->GetModel()->GetParameterMaximumValue(i)
		});
	}

	return paramMinMax;
}
