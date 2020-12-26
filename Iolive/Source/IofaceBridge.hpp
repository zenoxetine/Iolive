#pragma once

#include "Ioface/Ioface.hpp"
#include "Live2D/Live2DManager.hpp"
#include "GUI/Widget/ParameterGui.hpp"
#include "MathUtils.hpp"
#include "Logger.hpp"

namespace Iolive {
	class IofaceBridge
	{
	public:
		static void UpdateIoface()
		{
			s_Ioface.UpdateAll();
		}

		static void DoOptimizeParameters()
		{
			if (s_Ioface.IsCameraOpened() && !s_Ioface.IsLandmarksEmpty())
			{
				float deltaTime = static_cast<float>(Window::GetDeltaTime());
				#define SMOOTH_SLOW(start, end) MathUtils::Lerp(start, end, deltaTime * 5.f)
				#define SMOOTH_MEDIUM(start, end) MathUtils::Lerp(start, end, deltaTime * 10.f)
				#define SMOOTH_FAST(start, end) MathUtils::Lerp(start, end, deltaTime * 25.f)

				// ParamAngle
				OptimizedParameter.ParamAngleX = SMOOTH_SLOW(OptimizedParameter.ParamAngleX, s_Ioface.DistScale * s_Ioface.AngleX);
				OptimizedParameter.ParamAngleY = SMOOTH_SLOW(OptimizedParameter.ParamAngleY, s_Ioface.DistScale * s_Ioface.AngleY);
				OptimizedParameter.ParamAngleZ = SMOOTH_SLOW(OptimizedParameter.ParamAngleZ, s_Ioface.DistScale * s_Ioface.AngleZ);

				// mouth open y
				float absAngleY = MathUtils::Abs(OptimizedParameter.ParamAngleY);
				float normalizedMouthOpenY = MathUtils::Normalize(s_Ioface.DistScale * s_Ioface.MouthOpenY, 3.0f, 16.f);
				OptimizedParameter.ParamMouthOpenY = SMOOTH_FAST(OptimizedParameter.ParamMouthOpenY, normalizedMouthOpenY);

				// mouth form
				float normalizedMouthForm = MathUtils::Normalize(s_Ioface.DistScale * s_Ioface.MouthForm, 73.0f, 90.0f);
				OptimizedParameter.ParamMouthForm = SMOOTH_MEDIUM(OptimizedParameter.ParamMouthForm, normalizedMouthForm);

				// eye brow
				float optBrowLY = MathUtils::Normalize(s_Ioface.DistScale * s_Ioface.EyeBrowLY, 45.0f, 55.0f);
				float optBrowRY = MathUtils::Normalize(s_Ioface.DistScale * s_Ioface.EyeBrowRY, 45.0f, 55.0f);
				float avgBrow = (optBrowLY + optBrowRY) / 2.f;
				OptimizedParameter.ParamBrowLY = SMOOTH_SLOW(OptimizedParameter.ParamBrowLY, avgBrow);
				OptimizedParameter.ParamBrowRY = OptimizedParameter.ParamBrowLY; // same

				// eye
				float optEAR = MathUtils::Normalize(s_Ioface.DistScale * s_Ioface.EAR, 0.1f, 0.26f);
				OptimizedParameter.ParamEyeLOpen = SMOOTH_FAST(OptimizedParameter.ParamEyeLOpen, optEAR);
				OptimizedParameter.ParamEyeROpen = OptimizedParameter.ParamEyeLOpen;
			}
		}

		static bool OpenCamera(int deviceId) { return s_Ioface.OpenCamera(deviceId); }
		static void CloseCamera() { s_Ioface.CloseCamera(); }

		static bool IsCameraOpened() { return s_Ioface.IsCameraOpened(); }

	public:
		/*** Parameter binding ***/
		
		static void IofaceBridge::BindDefaultParametersWithFace()
		{
			const auto& paramIndex = Live2DManager::IndexOfDefaultParameter;
			
			// check is parameter exist?, then bind it
			if (paramIndex.ParamAngleX > -1)
				Live2DManager::SetParameterBinding(paramIndex.ParamAngleX, &(OptimizedParameter.ParamAngleX));
			if (paramIndex.ParamAngleY > -1)
				Live2DManager::SetParameterBinding(paramIndex.ParamAngleY, &(OptimizedParameter.ParamAngleY));
			if (paramIndex.ParamAngleZ > -1)
				Live2DManager::SetParameterBinding(paramIndex.ParamAngleZ, &(OptimizedParameter.ParamAngleZ));
			if (paramIndex.ParamEyeLOpen > -1)
				Live2DManager::SetParameterBinding(paramIndex.ParamEyeLOpen, &(OptimizedParameter.ParamEyeLOpen));
			if (paramIndex.ParamEyeROpen > -1)
				Live2DManager::SetParameterBinding(paramIndex.ParamEyeROpen, &(OptimizedParameter.ParamEyeROpen));
			if (paramIndex.ParamMouthOpenY > -1)
				Live2DManager::SetParameterBinding(paramIndex.ParamMouthOpenY, &(OptimizedParameter.ParamMouthOpenY));
			if (paramIndex.ParamMouthForm > -1)
				Live2DManager::SetParameterBinding(paramIndex.ParamMouthForm, &(OptimizedParameter.ParamMouthForm));
			if (paramIndex.ParamBrowLY > -1)
				Live2DManager::SetParameterBinding(paramIndex.ParamBrowLY, &(OptimizedParameter.ParamBrowLY));
			if (paramIndex.ParamBrowRY > -1)
				Live2DManager::SetParameterBinding(paramIndex.ParamBrowRY, &(OptimizedParameter.ParamBrowRY));
		}

		static void IofaceBridge::BindDefaultParametersWithGui()
		{
			const auto& paramIndex = Live2DManager::IndexOfDefaultParameter;

			// check is parameter exist?, then bind it
			if (paramIndex.ParamAngleX > -1)
				Live2DManager::SetParameterBinding(paramIndex.ParamAngleX, ParameterGui::GetPtrValueByIndex(paramIndex.ParamAngleX));
			if (paramIndex.ParamAngleY > -1)
				Live2DManager::SetParameterBinding(paramIndex.ParamAngleY, ParameterGui::GetPtrValueByIndex(paramIndex.ParamAngleY));
			if (paramIndex.ParamAngleZ > -1)
				Live2DManager::SetParameterBinding(paramIndex.ParamAngleZ, ParameterGui::GetPtrValueByIndex(paramIndex.ParamAngleZ));
			if (paramIndex.ParamEyeLOpen > -1)
				Live2DManager::SetParameterBinding(paramIndex.ParamEyeLOpen, ParameterGui::GetPtrValueByIndex(paramIndex.ParamEyeLOpen));
			if (paramIndex.ParamEyeROpen > -1)
				Live2DManager::SetParameterBinding(paramIndex.ParamEyeROpen, ParameterGui::GetPtrValueByIndex(paramIndex.ParamEyeROpen));
			if (paramIndex.ParamMouthOpenY > -1)
				Live2DManager::SetParameterBinding(paramIndex.ParamMouthOpenY, ParameterGui::GetPtrValueByIndex(paramIndex.ParamMouthOpenY));
			if (paramIndex.ParamMouthForm > -1)
				Live2DManager::SetParameterBinding(paramIndex.ParamMouthForm, ParameterGui::GetPtrValueByIndex(paramIndex.ParamMouthForm));
			if (paramIndex.ParamBrowLY > -1)
				Live2DManager::SetParameterBinding(paramIndex.ParamBrowLY, ParameterGui::GetPtrValueByIndex(paramIndex.ParamBrowLY));
			if (paramIndex.ParamBrowRY > -1)
				Live2DManager::SetParameterBinding(paramIndex.ParamBrowRY, ParameterGui::GetPtrValueByIndex(paramIndex.ParamBrowRY));
		}

	public:
		inline static struct Live2DManager::ParameterValue OptimizedParameter = {};

	private:
		inline static Ioface s_Ioface = Ioface(); // initialize directly
	};
}