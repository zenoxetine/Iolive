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
	inline static struct DefaultParameter {
		int ParamAngleX = -1;
		int ParamAngleY = -1;
		int ParamAngleZ = -1;
	} IndexOfDefaultParameter;

private:
	// Cubism memory allocator
	inline static LAppAllocator s_CubismAllocator;

	// for now only use 1 model
	inline static Model2D* s_Model2D;

	// variable binding for update model parameter
	// <ParameterIndex, PointerToFloat>
	inline static std::map<int, float*> s_ParameterBinding;

	// flags
	inline static bool s_IsModelChanged;
};