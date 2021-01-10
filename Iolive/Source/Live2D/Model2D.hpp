#pragma once

#include <GL/glew.h>
#include <CubismFramework.hpp>
#include <Model/CubismUserModel.hpp>
#include <ICubismModelSetting.hpp>
#include <Math/CubismMatrix44.hpp>
#include <Rendering/CubismRenderer.hpp>
#include <Rendering/OpenGL/CubismRenderer_OpenGLES2.hpp>
#include <Motion/ACubismMotion.hpp>
#include <Motion/CubismMotion.hpp>
#include <Motion/CubismMotionQueueEntry.hpp>
#include <Motion/CubismMotionQueueManager.hpp>
#include <Id/CubismId.hpp>
#include <Type/csmVector.hpp>
#include "Utility.hpp"
#include "Component/TextureManager.hpp"
#include <string>
#include <vector>
#include <array>
#include <map>

using namespace Csm;

namespace DefaultParameter {
	/*
	* unindexed parameter will be initialized with -1
	*/
	struct ParametersIndex {
		int ParamAngleX = -1;
		int ParamAngleY = -1;
		int ParamAngleZ = -1;
		int ParamBodyAngleX = -1;
		int ParamBodyAngleY = -1;
		int ParamBodyAngleZ = -1;
		int ParamEyeLOpen = -1;
		int ParamEyeROpen = -1;
		int ParamEyeLSmile = -1;
		int ParamEyeRSmile = -1;
		int ParamEyeForm = -1;
		int ParamEyeBallX = -1;
		int ParamEyeBallY = -1;
		int ParamMouthOpenY = -1;
		int ParamMouthForm = -1;
		int ParamBrowLY = -1;
		int ParamBrowRY = -1;
		int ParamBrowLForm = -1;
		int ParamBrowRForm = -1;
		int ParamBrowLAngle = -1;
		int ParamBrowRAngle = -1;
		int ParamBreath = -1;
	};

	/*
	* Default parameter value
	*/
	struct ParametersValue {
		float ParamAngleX = 0.0f;
		float ParamAngleY = 0.0f;
		float ParamAngleZ = 0.0f;
		float ParamBodyAngleX = 0.0f;
		float ParamBodyAngleY = 0.0f;
		float ParamBodyAngleZ = 0.0f;
		float ParamEyeLOpen = 1.0f;
		float ParamEyeROpen = 1.0f;
		float ParamEyeLSmile = 0.0f;
		float ParamEyeRSmile = 0.0f;
		float ParamEyeForm = 0.0f;
		float ParamEyeBallX = 0.0f;
		float ParamEyeBallY = 0.0f;
		float ParamMouthOpenY = 0.0f;
		float ParamMouthForm = 1.0f;
		float ParamBrowLY = 0.0f;
		float ParamBrowRY = 0.0f;
		float ParamBrowLForm = 0.0f;
		float ParamBrowRForm = 0.0f;
		float ParamBrowLAngle = 0.0f;
		float ParamBrowRAngle = 0.0f;
		float ParamBreath = 0.0f;
	};
}

struct ModelMotion
{
public:
	enum class MotionType { Motion = 0, Expression };

public:
	ModelMotion(int _id, const char* _name, ACubismMotion* _motion, MotionType _motionType)
		: id(_id), name(_name), motion(_motion), motionType(_motionType)
	{}

	const char* name;
	ACubismMotion* motion;
	MotionType motionType;
	int id;
};

typedef std::map<int, float*> ParameterBinding;

class Model2D : public CubismUserModel
{
public:
	Model2D(ICubismModelSetting* modelSetting, const std::wstring& modelDir, const std::wstring& modelFilename);
	~Model2D();

	void OnUpdate(float deltaTime);
	void OnDraw(int width, int height);

	void StartMotion(ModelMotion* motion);
	void ResetAllMotions();

private:
	bool SetupModelSetting(ICubismModelSetting* modelSetting);
	void SetupIndexOfDefaultParameters();
	void SetupModelUtils();

	void UpdateBindedParameters();

	void DoStartExpression(ACubismMotion* motion);
	void DoStartMotion(ACubismMotion* motion);

public:
	// <ParameterName, value>
	std::map<const char*, float> GetParameterMap();

	// [Index] => <Min, Max>
	std::vector<std::array<float, 2>> GetParameterMinMax();

	ParameterBinding& GetBindedParameter();
	DefaultParameter::ParametersIndex& GetParameterIndex();

	std::vector<ModelMotion>& GetExpressions();
	std::vector<ModelMotion>& GetMotions();

	int GetParameterCount() const;

	void SetParameterBinding(const ParameterBinding& parameterBinding);
	void SetParameterBindingAt(int index, float* ptrValue);

	void SetModelScale(float scaleValue);
	void SetModelTranslateX(float translateValue);
	void SetModelTranslateY(float translateValue);
	
	void AddModelScale(float scaleValue);
	void AddModelTranslateX(float translateValue);
	void AddModelTranslateY(float translateValue);

	float GetModelScale() const;
	float GetModelTranslateX() const;
	float GetModelTranslateY() const;

	CubismMatrix44* GetProjectionMatrix();

	const std::wstring& GetModelDir() const;
	const std::wstring& GetModelFileName() const;

private:
	std::wstring m_ModelDir;       // absolute model path
	std::wstring m_ModelFileName;  // model file name

	ICubismModelSetting* m_ModelSetting;
	TextureManager m_TextureManager;

	ParameterBinding m_ParameterBinding;
	DefaultParameter::ParametersIndex m_IndexOfDefaultParameter;

	csmVector<CubismIdHandle> m_EyeBlinkIds;
	csmVector<CubismIdHandle> m_LipSyncIds;

	std::vector<ModelMotion> m_Expressions;
	std::vector<ModelMotion> m_Motions;

	std::map<ACubismMotion*, CubismMotionQueueEntryHandle> m_MapActiveExpression;
	
	CubismMatrix44 m_ProjectionMatrix;
	float m_ModelScale;
	float m_ModelTranslateX;
	float m_ModelTranslateY;
};