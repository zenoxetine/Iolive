#include "Model2D.hpp"
#include <future>
#include <string.h>

Model2D::Model2D(ICubismModelSetting* modelSetting, const std::wstring& modelDir, const std::wstring& modelFilename)
	: m_ModelSetting(nullptr),
	m_ProjectionMatrix(CubismMatrix44()),
	m_ModelDir(modelDir),
	m_ModelFileName(modelFilename),
	m_ModelScale(1.0f),
	m_ModelTranslateX(0.0f),
	m_ModelTranslateY(0.0f)
{
	_initialized = false;

	if (SetupModelSetting(modelSetting))
	{
		_initialized = true;
	}
}

Model2D::~Model2D()
{
	CubismFramework::CoreLogFunction("[Model2D][I] Delete model!\n\n");

	// release expression
	for (auto& expression : m_Expressions)
		ACubismMotion::Delete(expression.motion);
	m_Expressions.clear();

	// release motions
	for (auto& motion : m_Motions)
		ACubismMotion::Delete(motion.motion);
	m_Motions.clear();

	m_MapActiveExpression.clear();

	// release model setting
	if (m_ModelSetting != nullptr)
		delete m_ModelSetting;
}

void Model2D::OnUpdate(float deltaTime)
{
	if (!_initialized || _model == NULL) return;

	UpdateBindedParameters();
	_model->LoadParameters();

	_motionManager->UpdateMotion(_model, deltaTime);
	_model->SaveParameters();
	_model->LoadParameters();

	_expressionManager->UpdateMotion(_model, deltaTime);

	if (_breath)
	{
		_breath->UpdateParameters(_model, deltaTime);
	}

	if (_physics)
	{
		_physics->Evaluate(_model, deltaTime);
	}

	if (_pose)
	{
		_pose->UpdateParameters(_model, deltaTime);
	}

	_model->Update();
}

void Model2D::OnDraw(int width, int height)
{
	if (!_initialized || _model == NULL) return;
	if (width < 1 || height < 1) return;

	CubismMatrix44* projectionMatrix = GetProjectionMatrix();

	projectionMatrix->Scale(
		(static_cast<float>(height) / static_cast<float>(width)) * m_ModelScale,
		m_ModelScale
	);
	projectionMatrix->TranslateX(m_ModelTranslateX);
	projectionMatrix->TranslateY(m_ModelTranslateY);

	projectionMatrix->MultiplyByMatrix(_modelMatrix);

	GetRenderer<Rendering::CubismRenderer_OpenGLES2>()->SetMvpMatrix(projectionMatrix);
	GetRenderer<Rendering::CubismRenderer_OpenGLES2>()->DrawModel();
}

void Model2D::StartMotion(ModelMotion* modelMotion)
{
	if (modelMotion->motionType == ModelMotion::MotionType::Motion)
	{
		DoStartMotion(modelMotion->motion);
	}
	else if (modelMotion->motionType == ModelMotion::MotionType::Expression)
	{
		DoStartExpression(modelMotion->motion);
	}
}

void Model2D::ResetAllMotions()
{
	// Stop active expressions
	for (auto& [AMotion, queueEntryHandler] : m_MapActiveExpression)
	{
		if (queueEntryHandler != NULL)
		{
			// Stop motion
			auto queueEntry = _expressionManager->GetCubismMotionQueueEntry(queueEntryHandler);
			queueEntry->SetFadeout(AMotion->GetFadeOutTime());
			queueEntry->StartFadeout(AMotion->GetFadeOutTime(), _expressionManager->GetUserTimeSeconds());

			m_MapActiveExpression.erase(AMotion);
		}
	}

	// Stop motions
	_motionManager->StopAllMotions();
}

void Model2D::DoStartExpression(ACubismMotion* motion)
{
	for (auto& [AMotion, queueEntryHandler] : m_MapActiveExpression)
	{
		if (motion == AMotion)
		{
			if (queueEntryHandler != NULL)
			{
				// Stop motion
				auto queueEntry = _expressionManager->GetCubismMotionQueueEntry(queueEntryHandler);
				queueEntry->SetFadeout(AMotion->GetFadeOutTime());
				queueEntry->StartFadeout(AMotion->GetFadeOutTime(), _expressionManager->GetUserTimeSeconds());

				m_MapActiveExpression.erase(AMotion);

				return;
			}
		}
	}

	CubismMotionQueueEntryHandle newQueueEntryHandler = _expressionManager->StartMotion(motion, false, /*unused*/0.0f, false);
	m_MapActiveExpression[motion] = newQueueEntryHandler;
}

void Model2D::DoStartMotion(ACubismMotion* motion)
{
	CubismMotionQueueEntryHandle newQueueEntryHandler = _motionManager->StartMotion(motion, false, /*unused*/0.0f, true);
}

bool Model2D::SetupModelSetting(ICubismModelSetting* modelSetting)
{
	_updating = true;

	// set modeSetting as class member
	m_ModelSetting = modelSetting;

	std::future<bool> loadMoc = std::async(std::launch::async, [this]() -> bool {
		// load .moc3
		wchar_t* moc3Filename = Utility::NewWideChar(m_ModelSetting->GetModelFileName());
		auto [buffer, fileSize] = Utility::CreateBufferFromFile((m_ModelDir + moc3Filename).data());
		delete[] moc3Filename;
		if (buffer)
		{
			LoadModel(buffer, fileSize);
			delete[] buffer;
			return true;
		}
		return false;
	});

	std::future<void> loadPhysics = std::async(std::launch::async, [this]() -> void {
		// load physics
		if (strlen(m_ModelSetting->GetPhysicsFileName()) > 0)
		{
			wchar_t* physicsFilename = Utility::NewWideChar(m_ModelSetting->GetPhysicsFileName());
			auto [buffer, fileSize] = Utility::CreateBufferFromFile((m_ModelDir + physicsFilename).data());
			delete[] physicsFilename;
			if (buffer)
			{
				LoadPhysics(buffer, fileSize);
				delete[] buffer;
			}
		}
	});

	std::future<void> loadExpressions = std::async(std::launch::async, [this]() -> void {
		// Load expressions
		const csmInt32 count = m_ModelSetting->GetExpressionCount();
		if (count > 0)
		{
			m_Expressions.reserve(count);
			for (csmInt32 i = 0; i < count; i++)
			{
				wchar_t* expressionPath = Utility::NewWideChar(m_ModelSetting->GetExpressionFileName(i));
				auto [buffer, fileSize] = Utility::CreateBufferFromFile((m_ModelDir + expressionPath).data());
				delete[] expressionPath;

				if (buffer)
				{
					ACubismMotion* motion = LoadExpression(buffer, fileSize, /*reinterpret_cast<char*>(expressionName)*/0);
					delete[] buffer;

					m_Expressions.push_back({
						i,
						m_ModelSetting->GetExpressionName(i),
						motion,
						ModelMotion::MotionType::Expression
					});
				}
			}
		}
	});

	// UserData
	if (strcmp(m_ModelSetting->GetUserDataFile(), "") != 0)
	{
		wchar_t* userDataFile = Utility::NewWideChar(m_ModelSetting->GetUserDataFile());
		auto[buffer, fileSize] = Utility::CreateBufferFromFile((m_ModelDir + userDataFile).data());
		delete[] userDataFile;

		if (buffer)
		{
			LoadUserData(buffer, fileSize);
			delete[] buffer;
		}
	}

	// pose
	if (strlen(m_ModelSetting->GetPoseFileName()) > 0)
	{
		wchar_t* poseFilename = Utility::NewWideChar(m_ModelSetting->GetPoseFileName());
		auto [buffer, fileSize] = Utility::CreateBufferFromFile((m_ModelDir + poseFilename).data());
		delete[] poseFilename;
		if (buffer)
		{
			LoadPose(buffer, fileSize);
			delete[] buffer;
		}
	}

	// Prepare texture
	bool textureErrFlags = false;
	for (csmInt32 modelTexCount = 0; modelTexCount < m_ModelSetting->GetTextureCount(); modelTexCount++)
	{
		wchar_t* textureFilename = Utility::NewWideChar(m_ModelSetting->GetTextureFileName(modelTexCount));
		if (wcslen(textureFilename) == 0)
		{
			delete[] textureFilename;
			continue;
		}

		std::wstring texturePath = m_ModelDir + textureFilename;
		delete[] textureFilename;

		if (!m_TextureManager.CreateTextureFromPngFile(texturePath.data()))
		{
			// some texture file may not found
			textureErrFlags = true;
			// breaakk, because the model will not showing or looks weird
			break;
		}
	}

	// wait until .moc3 loaded
	if (loadMoc.get() != true || !_moc)
	{
		// error while loading .moc3
		CubismFramework::CoreLogFunction("[Model2D][E] Error while loading .moc3 file\n");
		return false;
	}
	else
	{
		CubismFramework::CoreLogFunction("[Model2D][E] Model moc loaded\n");
	}

	if (textureErrFlags)
	{
		CubismFramework::CoreLogFunction("[Model2D][E] Error while loading texture file\n");
		return false;
	}

	// create renderer first
	CreateRenderer();

	// then bind texture into model
	for (csmInt32 modelTexCount = 0; modelTexCount < m_ModelSetting->GetTextureCount(); modelTexCount++)
	{
		const unsigned int textureId = m_TextureManager.GetTextureAt(modelTexCount);
		GetRenderer<Rendering::CubismRenderer_OpenGLES2>()->BindTexture(modelTexCount, textureId);
	}
	GetRenderer<Rendering::CubismRenderer_OpenGLES2>()->IsPremultipliedAlpha(false);

	csmMap<csmString, csmFloat32> modelLayout;
	m_ModelSetting->GetLayoutMap(modelLayout);
	_modelMatrix->SetupFromLayout(modelLayout);
	_model->SaveParameters();

	SetupIndexOfDefaultParameters();
	SetupModelUtils();

	// Preload model motions
	int motionId = 0;
	for (csmInt32 i = 0; i < m_ModelSetting->GetMotionGroupCount(); i++)
	{
		const char* groupName = m_ModelSetting->GetMotionGroupName(i);

		const csmInt32 count = m_ModelSetting->GetMotionCount(groupName);
		for (csmInt32 i = 0; i < count; i++)
		{
			wchar_t* motionPath = Utility::NewWideChar(m_ModelSetting->GetMotionFileName(groupName, i));
			auto [buffer, fileSize] = Utility::CreateBufferFromFile((m_ModelDir + motionPath).data());
			delete[] motionPath;

			if (buffer)
			{
				CubismMotion* motion = static_cast<CubismMotion*>(LoadMotion(buffer, fileSize, 0));
				delete[] buffer;

				float fadeInTime = m_ModelSetting->GetMotionFadeInTimeValue(groupName, i);
				float fadeOutTime = m_ModelSetting->GetMotionFadeOutTimeValue(groupName, i);

				if (fadeInTime >= 0.0f)
					motion->SetFadeInTime(fadeInTime);
				if (fadeOutTime >= 0.0f)
					motion->SetFadeOutTime(fadeOutTime);

				motion->SetEffectIds(m_EyeBlinkIds, m_LipSyncIds);

				m_Motions.push_back({
					motionId,
					m_ModelSetting->GetMotionFileName(groupName, i),
					motion,
					ModelMotion::MotionType::Motion
				});
			}
			motionId++;
		}
	}
	_motionManager->StopAllMotions();

	loadPhysics.wait();
	loadExpressions.wait();

	_updating = false;

	return true;
}

void Model2D::SetupIndexOfDefaultParameters()
{
	const char** paramIds = GetModel()->GetParameterIds();
	csmInt32 paramCount = GetModel()->GetParameterCount();

	for (csmInt32 paramIndex = 0; paramIndex < paramCount; paramIndex++)
	{
		if (strcmp(paramIds[paramIndex], "PARAM_ANGLE_X") == 0 ||
			strcmp(paramIds[paramIndex], "ParamAngleX") == 0)
			m_IndexOfDefaultParameter.ParamAngleX = paramIndex;

		else if (strcmp(paramIds[paramIndex], "PARAM_ANGLE_Y") == 0 ||
			strcmp(paramIds[paramIndex], "ParamAngleY") == 0)
			m_IndexOfDefaultParameter.ParamAngleY = paramIndex;

		else if (strcmp(paramIds[paramIndex], "PARAM_ANGLE_Z") == 0 ||
			strcmp(paramIds[paramIndex], "ParamAngleZ") == 0)
			m_IndexOfDefaultParameter.ParamAngleZ = paramIndex;

		else if (strcmp(paramIds[paramIndex], "PARAM_BODY_ANGLE_X") == 0 ||
			strcmp(paramIds[paramIndex], "ParamBodyAngleX") == 0)
			m_IndexOfDefaultParameter.ParamBodyAngleX = paramIndex;

		else if (strcmp(paramIds[paramIndex], "PARAM_BODY_ANGLE_Y") == 0 ||
			strcmp(paramIds[paramIndex], "ParamBodyAngleY") == 0)
			m_IndexOfDefaultParameter.ParamBodyAngleY = paramIndex;

		else if (strcmp(paramIds[paramIndex], "PARAM_BODY_ANGLE_Z") == 0 ||
			strcmp(paramIds[paramIndex], "ParamBodyAngleZ") == 0)
			m_IndexOfDefaultParameter.ParamBodyAngleZ = paramIndex;

		else if (strcmp(paramIds[paramIndex], "PARAM_EYE_L_OPEN") == 0 ||
			strcmp(paramIds[paramIndex], "ParamEyeLOpen") == 0)
			m_IndexOfDefaultParameter.ParamEyeLOpen = paramIndex;

		else if (strcmp(paramIds[paramIndex], "PARAM_EYE_R_OPEN") == 0 ||
			strcmp(paramIds[paramIndex], "ParamEyeROpen") == 0)
			m_IndexOfDefaultParameter.ParamEyeROpen = paramIndex;

		else if (strcmp(paramIds[paramIndex], "PARAM_EYE_L_FORM") == 0 ||
			strcmp(paramIds[paramIndex], "ParamEyeLSmile") == 0)
			m_IndexOfDefaultParameter.ParamEyeLSmile = paramIndex;

		else if (strcmp(paramIds[paramIndex], "PARAM_EYE_R_FORM") == 0 ||
			strcmp(paramIds[paramIndex], "ParamEyeRSmile") == 0)
			m_IndexOfDefaultParameter.ParamEyeRSmile = paramIndex;

		else if (strcmp(paramIds[paramIndex], "PARAM_EYE_FORM") == 0 ||
			strcmp(paramIds[paramIndex], "ParamEyeForm") == 0)
			m_IndexOfDefaultParameter.ParamEyeForm = paramIndex;

		else if (strcmp(paramIds[paramIndex], "PARAM_EYE_BALL_X") == 0 ||
			strcmp(paramIds[paramIndex], "ParamEyeBallX") == 0)
			m_IndexOfDefaultParameter.ParamEyeBallX = paramIndex;

		else if (strcmp(paramIds[paramIndex], "PARAM_EYE_BALL_Y") == 0 ||
			strcmp(paramIds[paramIndex], "ParamEyeBallY") == 0)
			m_IndexOfDefaultParameter.ParamEyeBallY = paramIndex;

		else if (strcmp(paramIds[paramIndex], "PARAM_MOUTH_OPEN_Y") == 0 ||
			strncmp(paramIds[paramIndex], "ParamMouthOpen", 14) == 0)
			m_IndexOfDefaultParameter.ParamMouthOpenY = paramIndex;

		else if (strcmp(paramIds[paramIndex], "PARAM_MOUTH_FORM") == 0 ||
			strcmp(paramIds[paramIndex], "ParamMouthForm") == 0)
			m_IndexOfDefaultParameter.ParamMouthForm = paramIndex;

		else if (strcmp(paramIds[paramIndex], "PARAM_BROW_L_Y") == 0 ||
			strcmp(paramIds[paramIndex], "ParamBrowLY") == 0)
			m_IndexOfDefaultParameter.ParamBrowLY = paramIndex;

		else if (strcmp(paramIds[paramIndex], "PARAM_BROW_R_Y") == 0 ||
			strcmp(paramIds[paramIndex], "ParamBrowRY") == 0)
			m_IndexOfDefaultParameter.ParamBrowRY = paramIndex;

		else if (strcmp(paramIds[paramIndex], "PARAM_BROW_L_FORM") == 0 ||
			strcmp(paramIds[paramIndex], "ParamBrowLForm") == 0)
			m_IndexOfDefaultParameter.ParamBrowLForm = paramIndex;

		else if (strcmp(paramIds[paramIndex], "PARAM_BROW_R_FORM") == 0 ||
			strcmp(paramIds[paramIndex], "ParamBrowRForm") == 0)
			m_IndexOfDefaultParameter.ParamBrowRForm = paramIndex;

		else if (strcmp(paramIds[paramIndex], "PARAM_BROW_L_ANGLE") == 0 ||
			strcmp(paramIds[paramIndex], "ParamBrowLAngle") == 0)
			m_IndexOfDefaultParameter.ParamBrowLAngle = paramIndex;

		else if (strcmp(paramIds[paramIndex], "PARAM_BROW_R_ANGLE") == 0 ||
			strcmp(paramIds[paramIndex], "ParamBrowRAngle") == 0)
			m_IndexOfDefaultParameter.ParamBrowRAngle = paramIndex;

		else if (strcmp(paramIds[paramIndex], "PARAM_BREATH") == 0 ||
			strcmp(paramIds[paramIndex], "ParamBreath") == 0)
			m_IndexOfDefaultParameter.ParamBreath = paramIndex;
	}
}

void Model2D::SetupModelUtils()
{
	csmVector<CubismIdHandle> _ids = GetModel()->GetParameterIdHandles();

	// Setup breath
	if (m_IndexOfDefaultParameter.ParamBreath > -1)
	{
		_breath = CubismBreath::Create();
		csmVector<CubismBreath::BreathParameterData> breathParameters;

		CubismIdHandle idParamBreath = _ids[m_IndexOfDefaultParameter.ParamBreath];
		breathParameters.PushBack(CubismBreath::BreathParameterData(idParamBreath, 0.5f, 0.5f, 3.8f, 0.5f));

		// set breath data
		_breath->SetParameters(breathParameters);
	}

	// setup eyeblink
	_eyeBlink = CubismEyeBlink::Create(m_ModelSetting);
	if (m_ModelSetting->GetEyeBlinkParameterCount() > 0)
	{
		// eyeblink using parameter from model setting
		for (csmInt32 i = 0; i < m_ModelSetting->GetEyeBlinkParameterCount(); i++)
			m_EyeBlinkIds.PushBack(m_ModelSetting->GetEyeBlinkParameterId(i));
	}
	else {
		// eyeblink using default eye parameter
		if (m_IndexOfDefaultParameter.ParamEyeLOpen > -1)
			m_EyeBlinkIds.PushBack(_ids[m_IndexOfDefaultParameter.ParamEyeLOpen]);

		if (m_IndexOfDefaultParameter.ParamEyeROpen > -1)
			m_EyeBlinkIds.PushBack(_ids[m_IndexOfDefaultParameter.ParamEyeROpen]);
	}
	_eyeBlink->SetParameterIds(m_EyeBlinkIds);

	// pass LipSync Ids
	csmInt32 lipSyncIdCount = m_ModelSetting->GetLipSyncParameterCount();
	for (csmInt32 i = 0; i < lipSyncIdCount; ++i)
	{
		m_LipSyncIds.PushBack(m_ModelSetting->GetLipSyncParameterId(i));
	}
}

void Model2D::UpdateBindedParameters()
{
	// update binded parameter
	for (auto [index, ptrValue] : m_ParameterBinding)
	{
		if (ptrValue)
		{
			GetModel()->SetParameterValue(index, *ptrValue);
		}
	}

	GetModel()->SaveParameters();
}

/*
* Getter & Setter
*/

std::map<const char*, float> Model2D::GetParameterMap()
{
	const char** paramIds = GetModel()->GetParameterIds();
	csmInt32 paramCount = GetModel()->GetParameterCount();

	std::map<const char*, float> parameterMap;

	for (csmInt32 i = 0; i < paramCount; i++)
	{
		parameterMap.insert(std::pair<const char*, float>(paramIds[i], GetModel()->GetParameterValue(i)));
	}

	return parameterMap;
}

std::vector<std::array<float, 2>> Model2D::GetParameterMinMax()
{
	csmInt32 paramCount = GetModel()->GetParameterCount();

	std::vector<std::array<float, 2>> paramMinMax;
	paramMinMax.reserve(paramCount);

	for (csmInt32 i = 0; i < paramCount; i++)
	{
		paramMinMax.push_back({
			GetModel()->GetParameterMinimumValue(i),
			GetModel()->GetParameterMaximumValue(i)
		});
	}

	return paramMinMax;
}

ParameterBinding& Model2D::GetBindedParameter() { return m_ParameterBinding; }
DefaultParameter::ParametersIndex& Model2D::GetParameterIndex() { return m_IndexOfDefaultParameter; }

std::vector<ModelMotion>& Model2D::GetExpressions() { return m_Expressions; }
std::vector<ModelMotion>& Model2D::GetMotions() { return m_Motions; }

int Model2D::GetParameterCount() const { return GetModel()->GetParameterCount(); }

void Model2D::SetParameterBinding(const ParameterBinding& parameterBinding) { m_ParameterBinding = parameterBinding; }
void Model2D::SetParameterBindingAt(int index, float* ptrValue) { m_ParameterBinding[index] = ptrValue; };

void Model2D::SetModelScale(float scaleValue) { m_ModelScale = scaleValue; }
void Model2D::AddModelScale(float scaleValue) { m_ModelScale += scaleValue; }
float Model2D::GetModelScale() const { return m_ModelScale; }

void Model2D::SetModelTranslateX(float translateValue) { m_ModelTranslateX = translateValue; }
void Model2D::AddModelTranslateX(float translateValue) { m_ModelTranslateX += translateValue; }
float Model2D::GetModelTranslateX() const { return m_ModelTranslateX; };

void Model2D::SetModelTranslateY(float translateValue) { m_ModelTranslateY = translateValue; }
void Model2D::AddModelTranslateY(float translateValue) { m_ModelTranslateY += translateValue; }
float Model2D::GetModelTranslateY() const { return m_ModelTranslateY; };

CubismMatrix44* Model2D::GetProjectionMatrix() { return &m_ProjectionMatrix; }

const std::wstring& Model2D::GetModelDir() const { return m_ModelDir; };
const std::wstring& Model2D::GetModelFileName() const { return m_ModelFileName; };