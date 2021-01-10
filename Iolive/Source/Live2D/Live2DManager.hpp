#pragma once

#include <CubismFramework.hpp>
#include <ICubismModelSetting.hpp>
#include "Model2D.hpp"
#include "CubismSamples/LAppAllocator.hpp"

class Live2DManager
{
public:
	Live2DManager(const Live2DManager&) = delete;
	Live2DManager() = delete;

	static bool InitCubism();
	static void ReleaseCubism();

	static Model2D* CreateModel(const wchar_t* modelJson);

private:
	static bool CheckModelSetting(ICubismModelSetting* modelSetting);

public:
	// log function
	using FPLogFunc = void(*)(const char*, ...);
	inline static FPLogFunc LoggingFunction = [](const char*, ...) {};

private:
	// Cubism memory allocator
	inline static LAppAllocator s_CubismAllocator = LAppAllocator();;

	inline static CubismFramework::Option s_CubismOption = CubismFramework::Option();
};
