#include "Live2DManager.hpp"
#include "Utility.hpp"
#include <filesystem>
#include <string.h>

bool Live2DManager::Init()
{
	if (!CubismFramework::IsInitialized())
	{
		// Initialize CubismFramework
		s_CubismOption.LoggingLevel = CubismFramework::Option::LogLevel_Verbose;
		s_CubismOption.LogFunction = [](const char* message) { s_LogFunc(message); };
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
	s_LogFunc("[Live2DManager][I] Creating new model ...\n");
	s_Model2D = new Model2D(modelSetting, modelDir.data(), modelFilename.data());
	if (s_Model2D->IsInitialized())
	{
		s_LogFunc("[Live2DManager][I] New model initialized\n");
		SetupIndexOfDefaultParameter();
		s_IsModelChanged = true; // signal
		return true;
	}
	else
	{
		s_LogFunc("[Live2DManager][E] Error while creating new model\n");
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
		s_LogFunc("[Live2DManager][E] Can't found .moc3 from the json file\n");
		return false;
	}

	for (int i = 0; i < strlen(mocFilename); i++)
	{
		if (mocFilename[i] < 0)
		{
			// seems "model filename" contains unicode value, ex: japanese text
			// JsonParser from CubismFramework doesn't support wchar_t and will return an unexpected value
			s_LogFunc("[Live2DManager][E] Can't load model that contains a unicode in the filename\n");
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
	
		else if(strcmp(paramIds[paramIndex], "PARAM_BODY_ANGLE_X") == 0 ||
				strcmp(paramIds[paramIndex], "ParamBodyAngleX") == 0)
			IndexOfDefaultParameter.ParamBodyAngleX = paramIndex;

		else if (strcmp(paramIds[paramIndex], "PARAM_BODY_ANGLE_Y") == 0 ||
				 strcmp(paramIds[paramIndex], "ParamBodyAngleY") == 0)
			IndexOfDefaultParameter.ParamBodyAngleY = paramIndex;
		
		else if (strcmp(paramIds[paramIndex], "PARAM_BODY_ANGLE_Z") == 0 ||
				 strcmp(paramIds[paramIndex], "ParamBodyAngleZ") == 0)
			IndexOfDefaultParameter.ParamBodyAngleZ = paramIndex;

		else if (strcmp(paramIds[paramIndex], "PARAM_EYE_L_OPEN") == 0 ||
			strcmp(paramIds[paramIndex], "ParamEyeLOpen") == 0)
			IndexOfDefaultParameter.ParamEyeLOpen = paramIndex;

		else if (strcmp(paramIds[paramIndex], "PARAM_EYE_R_OPEN") == 0 ||
			strcmp(paramIds[paramIndex], "ParamEyeROpen") == 0)
			IndexOfDefaultParameter.ParamEyeROpen = paramIndex;
		
		else if (strcmp(paramIds[paramIndex], "PARAM_EYE_L_FORM") == 0 ||
			strcmp(paramIds[paramIndex], "ParamEyeLSmile") == 0)
			IndexOfDefaultParameter.ParamEyeLSmile = paramIndex;

		else if (strcmp(paramIds[paramIndex], "PARAM_EYE_R_FORM") == 0 ||
			strcmp(paramIds[paramIndex], "ParamEyeRSmile") == 0)
			IndexOfDefaultParameter.ParamEyeRSmile = paramIndex;
		
		else if (strcmp(paramIds[paramIndex], "PARAM_EYE_FORM") == 0 ||
			strcmp(paramIds[paramIndex], "ParamEyeForm") == 0)
			IndexOfDefaultParameter.ParamEyeForm = paramIndex;
		
		else if (strcmp(paramIds[paramIndex], "PARAM_EYE_BALL_X") == 0 ||
			strcmp(paramIds[paramIndex], "ParamEyeBallX") == 0)
			IndexOfDefaultParameter.ParamEyeBallX = paramIndex;

		else if (strcmp(paramIds[paramIndex], "PARAM_EYE_BALL_Y") == 0 ||
			strcmp(paramIds[paramIndex], "ParamEyeBallY") == 0)
			IndexOfDefaultParameter.ParamEyeBallY = paramIndex;

		else if (strcmp(paramIds[paramIndex], "PARAM_MOUTH_OPEN_Y") == 0 ||
			strncmp(paramIds[paramIndex], "ParamMouthOpen", 14) == 0)
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
		
		else if (strcmp(paramIds[paramIndex], "PARAM_BROW_L_FORM") == 0 ||
			strcmp(paramIds[paramIndex], "ParamBrowLForm") == 0)
			IndexOfDefaultParameter.ParamBrowLForm = paramIndex;
		
		else if (strcmp(paramIds[paramIndex], "PARAM_BROW_R_FORM") == 0 ||
			strcmp(paramIds[paramIndex], "ParamBrowRForm") == 0)
			IndexOfDefaultParameter.ParamBrowRForm = paramIndex;
		
		else if (strcmp(paramIds[paramIndex], "PARAM_BROW_L_ANGLE") == 0 ||
			strcmp(paramIds[paramIndex], "ParamBrowLAngle") == 0)
			IndexOfDefaultParameter.ParamBrowLAngle = paramIndex;
		
		else if (strcmp(paramIds[paramIndex], "PARAM_BROW_R_ANGLE") == 0 ||
			strcmp(paramIds[paramIndex], "ParamBrowRAngle") == 0)
			IndexOfDefaultParameter.ParamBrowRAngle = paramIndex;
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
		s_LogFunc("[Live2DManager][I] Delete model ...\n");
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
	if (width < 1 || height < 1) return;
	if (IsModelInitialized())
	{
		// scale & translate model matrix
		CubismMatrix44* projectionMatrix = s_Model2D->GetProjectionMatrix();
		projectionMatrix->Scale(
			(static_cast<float>(height) / static_cast<float>(width)) * s_ModelScale,
			s_ModelScale
		);
		projectionMatrix->TranslateX(s_ModelTransX);
		projectionMatrix->TranslateY(s_ModelTransY);

		// draw
		s_Model2D->OnDraw();
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
