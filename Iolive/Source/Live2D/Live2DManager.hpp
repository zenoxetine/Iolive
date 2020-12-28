#pragma once

#include <CubismFramework.hpp>
#include <ICubismModelSetting.hpp>
#include <map>
#include <vector>
#include <array>
#include "Model2D.hpp"
#include "CubismSamples/LAppAllocator.hpp"

class Live2DManager
{
public:
	static bool Init();
	static void Release();

	static bool SetModel(const wchar_t* modelJson);
	static bool IsModelChanged() { return s_IsModelChanged; }
	static bool IsModelInitialized();

	static void AddModelScale(float value) { s_ModelScale += value; }
	static void SetModelScale(float value) { s_ModelScale = value; }
	static float GetModelScale() { return s_ModelScale; }

	static void AddModelTranslateX(float value) { s_ModelTransX += value; }
	static void AddModelTranslateY(float value) { s_ModelTransY += value; }

	static void OnUpdate(float deltaTime);
	static void OnDraw(int width, int height);

	static void SetParameterBinding(int index, float* ptrValue);
	static void ReleaseAllParameterBinding();

	static std::map<const char*, float> GetParameterMap();
	static std::vector<std::array<float, 2>> GetParameterMinMax();

private:
	Live2DManager();
	static bool CheckModelSetting(ICubismModelSetting* modelSetting);
	static void SetupIndexOfDefaultParameter();
	static void TryDeleteModel();

public:
	/* 
	* unindexed parameter will be initialized with -1
	*/
	struct ParameterIndex{
		int ParamAngleX = -1;
		int ParamAngleY = -1;
		int ParamAngleZ = -1;
		int ParamEyeLOpen = -1;
		int ParamEyeROpen = -1;
		int ParamMouthOpenY = -1;
		int ParamMouthForm = -1;
		int ParamBrowLY = -1;
		int ParamBrowRY = -1;
	};
	
	/*
	* Default parameter value
	*/
	struct ParameterValue{
		float ParamAngleX = 0.0f;
		float ParamAngleY = 0.0f;
		float ParamAngleZ = 0.0f;
		float ParamEyeLOpen = 1.0f;
		float ParamEyeROpen = 1.0f;
		float ParamMouthOpenY = 0.0f;
		float ParamMouthForm = 1.0f;
		float ParamBrowLY = 0.0f;
		float ParamBrowRY = 0.0f;
	};

	inline static struct ParameterIndex IndexOfDefaultParameter; // connected with s_Model

private:
	// Cubism memory allocator
	inline static LAppAllocator s_CubismAllocator;
	inline static CubismFramework::Option s_CubismOption;

	// only use 1 model
	inline static Model2D* s_Model2D;

	// value for scaling / translating model matrix
	inline static float s_ModelScale = 1.0f;
	inline static float s_ModelTransX = 0.0f;
	inline static float s_ModelTransY = 0.0f;

	// variable binding for update model parameter
	// <ParameterIndex, PointerToFloat>
	inline static std::map<int, float*> s_ParameterBinding;

	// signal
	inline static bool s_IsModelChanged;
};