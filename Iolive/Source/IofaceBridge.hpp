#pragma once

#include "Ioface/Ioface.hpp"
#include "Live2D/Live2DManager.hpp"
#include "GUI/Widget/MainWidget.hpp"
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
				#define SMOOTH_FAST(start, end) MathUtils::Lerp(start, end, deltaTime * 20.f)

				// ParamAngle
				OptimizedParameter.ParamAngleX = SMOOTH_SLOW(OptimizedParameter.ParamAngleX, s_Ioface.AngleX);
				OptimizedParameter.ParamAngleY = SMOOTH_SLOW(OptimizedParameter.ParamAngleY, s_Ioface.AngleY * 1.3f);
				OptimizedParameter.ParamAngleZ = SMOOTH_SLOW(OptimizedParameter.ParamAngleZ, s_Ioface.AngleZ);

				// BodyAngle
				OptimizedParameter.ParamBodyAngleX = OptimizedParameter.ParamAngleX * 0.2f;
				OptimizedParameter.ParamBodyAngleY = OptimizedParameter.ParamAngleY * 0.25f;
				OptimizedParameter.ParamBodyAngleZ = OptimizedParameter.ParamAngleZ * 0.2f;

				// mouth open y
				float normalizedMouthOpenY = MathUtils::Normalize(s_Ioface.DistScale * s_Ioface.MouthOpenY, 4.0f, 14.0f);
				OptimizedParameter.ParamMouthOpenY = SMOOTH_FAST(OptimizedParameter.ParamMouthOpenY, normalizedMouthOpenY);

				// mouth form
				float normalizedMouthForm = MathUtils::Normalize(s_Ioface.DistScale * s_Ioface.MouthForm, 70.0f, 87.0f);
				OptimizedParameter.ParamMouthForm = SMOOTH_FAST(OptimizedParameter.ParamMouthForm, normalizedMouthForm);

				// Eye Open
				float normalizedEAR = MathUtils::Normalize(s_Ioface.DistScale * s_Ioface.EAR, 0.115f, 0.23f);
				OptimizedParameter.ParamEyeLOpen = SMOOTH_MEDIUM(OptimizedParameter.ParamEyeLOpen, normalizedEAR);
				OptimizedParameter.ParamEyeROpen = OptimizedParameter.ParamEyeLOpen;

				// Eye smile based on MouthForm
				//OptimizedParameter.ParamEyeForm = MathUtils::Normalize(OptimizedParameter.ParamMouthForm, -0.3, 0.9f);
				//OptimizedParameter.ParamEyeLSmile = OptimizedParameter.ParamEyeForm; // both left | right are equal ^^
				//OptimizedParameter.ParamEyeRSmile = OptimizedParameter.ParamEyeForm;

				// Eye smile based on AngleY
				float normalizedEyeForm = MathUtils::Normalize(OptimizedParameter.ParamAngleY, -20.0f, 10.0f);
				OptimizedParameter.ParamEyeForm = SMOOTH_MEDIUM(OptimizedParameter.ParamEyeForm, normalizedEyeForm);
				OptimizedParameter.ParamEyeLSmile = OptimizedParameter.ParamEyeForm; // both left | right are equal ^^
				OptimizedParameter.ParamEyeRSmile = OptimizedParameter.ParamEyeForm;

				// EyeBrow Y
				float optBrowLY = MathUtils::Normalize(s_Ioface.DistScale * s_Ioface.EyeBrowLY, 46.0f, 54.0f);
				float optBrowRY = MathUtils::Normalize(s_Ioface.DistScale * s_Ioface.EyeBrowRY, 46.0f, 54.0f);
				float avgBrow = (optBrowLY + optBrowRY) / 2.f;
				OptimizedParameter.ParamBrowLY = SMOOTH_SLOW(OptimizedParameter.ParamBrowLY, avgBrow);
				OptimizedParameter.ParamBrowRY = OptimizedParameter.ParamBrowLY; // same

				// EyeBrow Form
				OptimizedParameter.ParamBrowLForm = OptimizedParameter.ParamBrowLY < 0.0f ? OptimizedParameter.ParamBrowLY : 0.0f;
				OptimizedParameter.ParamBrowRForm = OptimizedParameter.ParamBrowLForm;

				// EyeBrow Angle
				OptimizedParameter.ParamBrowLAngle = OptimizedParameter.ParamBrowLForm;
				OptimizedParameter.ParamBrowRAngle = OptimizedParameter.ParamBrowLForm;
			}
		}

		static bool OpenCamera(int deviceId) { return s_Ioface.OpenCamera(deviceId); }
		static void CloseCamera() { s_Ioface.CloseCamera(); }

		static bool IsCameraOpened() { return s_Ioface.IsCameraOpened(); }

	public:
		/*** Parameter binding ***/
		
		static void IofaceBridge::BindDefaultParametersWithFace()
		{
			MainWidget::LogScene.AddLog("[Iolive][I] Binding model parameters with Ioface\n");

			const auto& paramIndex = Live2DManager::IndexOfDefaultParameter;
			
			// check is parameter exist?, then bind it
			if (paramIndex.ParamAngleX > -1)
				Live2DManager::SetParameterBinding(paramIndex.ParamAngleX, &(OptimizedParameter.ParamAngleX));
			if (paramIndex.ParamAngleY > -1)
				Live2DManager::SetParameterBinding(paramIndex.ParamAngleY, &(OptimizedParameter.ParamAngleY));
			if (paramIndex.ParamAngleZ > -1)
				Live2DManager::SetParameterBinding(paramIndex.ParamAngleZ, &(OptimizedParameter.ParamAngleZ));
			if (paramIndex.ParamBodyAngleX > -1)
				Live2DManager::SetParameterBinding(paramIndex.ParamBodyAngleX, &(OptimizedParameter.ParamBodyAngleX));
			if (paramIndex.ParamBodyAngleY > -1)
				Live2DManager::SetParameterBinding(paramIndex.ParamBodyAngleY, &(OptimizedParameter.ParamBodyAngleY));
			if (paramIndex.ParamBodyAngleZ > -1)
				Live2DManager::SetParameterBinding(paramIndex.ParamBodyAngleZ, &(OptimizedParameter.ParamBodyAngleZ));
			if (paramIndex.ParamEyeLOpen > -1)
				Live2DManager::SetParameterBinding(paramIndex.ParamEyeLOpen, &(OptimizedParameter.ParamEyeLOpen));
			if (paramIndex.ParamEyeROpen > -1)
				Live2DManager::SetParameterBinding(paramIndex.ParamEyeROpen, &(OptimizedParameter.ParamEyeROpen));
			if (paramIndex.ParamEyeLSmile > -1)
				Live2DManager::SetParameterBinding(paramIndex.ParamEyeLSmile, &(OptimizedParameter.ParamEyeLSmile));
			if (paramIndex.ParamEyeRSmile > -1)
				Live2DManager::SetParameterBinding(paramIndex.ParamEyeRSmile, &(OptimizedParameter.ParamEyeRSmile));
			if (paramIndex.ParamEyeForm > -1)
				Live2DManager::SetParameterBinding(paramIndex.ParamEyeForm, &(OptimizedParameter.ParamEyeForm));
			if (paramIndex.ParamMouthOpenY > -1)
				Live2DManager::SetParameterBinding(paramIndex.ParamMouthOpenY, &(OptimizedParameter.ParamMouthOpenY));
			if (paramIndex.ParamMouthForm > -1)
				Live2DManager::SetParameterBinding(paramIndex.ParamMouthForm, &(OptimizedParameter.ParamMouthForm));
			if (paramIndex.ParamBrowLY > -1)
				Live2DManager::SetParameterBinding(paramIndex.ParamBrowLY, &(OptimizedParameter.ParamBrowLY));
			if (paramIndex.ParamBrowRY > -1)
				Live2DManager::SetParameterBinding(paramIndex.ParamBrowRY, &(OptimizedParameter.ParamBrowRY));
			if (paramIndex.ParamBrowLForm > -1)
				Live2DManager::SetParameterBinding(paramIndex.ParamBrowLForm, &(OptimizedParameter.ParamBrowLForm));
			if (paramIndex.ParamBrowRForm > -1)
				Live2DManager::SetParameterBinding(paramIndex.ParamBrowRForm, &(OptimizedParameter.ParamBrowRForm));
			if (paramIndex.ParamBrowLAngle > -1)
				Live2DManager::SetParameterBinding(paramIndex.ParamBrowLAngle, &(OptimizedParameter.ParamBrowLAngle));
			if (paramIndex.ParamBrowRAngle > -1)
				Live2DManager::SetParameterBinding(paramIndex.ParamBrowRAngle, &(OptimizedParameter.ParamBrowRAngle));
		}

		static void IofaceBridge::BindDefaultParametersWithGui()
		{
			MainWidget::LogScene.AddLog("[Iolive][I] Binding model parameters with the GUI\n");

			const auto& paramIndex = Live2DManager::IndexOfDefaultParameter;

			// check is parameter exist?, then bind it
			if (paramIndex.ParamAngleX > -1)
				Live2DManager::SetParameterBinding(paramIndex.ParamAngleX, ParameterGui::GetPtrValueByIndex(paramIndex.ParamAngleX));
			if (paramIndex.ParamAngleY > -1)
				Live2DManager::SetParameterBinding(paramIndex.ParamAngleY, ParameterGui::GetPtrValueByIndex(paramIndex.ParamAngleY));
			if (paramIndex.ParamAngleZ > -1)
				Live2DManager::SetParameterBinding(paramIndex.ParamAngleZ, ParameterGui::GetPtrValueByIndex(paramIndex.ParamAngleZ));
			if (paramIndex.ParamBodyAngleX > -1)
				Live2DManager::SetParameterBinding(paramIndex.ParamBodyAngleX, ParameterGui::GetPtrValueByIndex(paramIndex.ParamBodyAngleX));
			if (paramIndex.ParamBodyAngleY > -1)
				Live2DManager::SetParameterBinding(paramIndex.ParamBodyAngleY, ParameterGui::GetPtrValueByIndex(paramIndex.ParamBodyAngleY));
			if (paramIndex.ParamBodyAngleZ > -1)
				Live2DManager::SetParameterBinding(paramIndex.ParamAngleZ, ParameterGui::GetPtrValueByIndex(paramIndex.ParamAngleZ));
			if (paramIndex.ParamEyeLOpen > -1)
				Live2DManager::SetParameterBinding(paramIndex.ParamEyeLOpen, ParameterGui::GetPtrValueByIndex(paramIndex.ParamEyeLOpen));
			if (paramIndex.ParamEyeROpen > -1)
				Live2DManager::SetParameterBinding(paramIndex.ParamEyeROpen, ParameterGui::GetPtrValueByIndex(paramIndex.ParamEyeROpen));
			if (paramIndex.ParamEyeLSmile > -1)
				Live2DManager::SetParameterBinding(paramIndex.ParamEyeLSmile, ParameterGui::GetPtrValueByIndex(paramIndex.ParamEyeLSmile));
			if (paramIndex.ParamEyeRSmile > -1)
				Live2DManager::SetParameterBinding(paramIndex.ParamEyeRSmile, ParameterGui::GetPtrValueByIndex(paramIndex.ParamEyeRSmile));
			if (paramIndex.ParamEyeForm > -1)
				Live2DManager::SetParameterBinding(paramIndex.ParamEyeForm, ParameterGui::GetPtrValueByIndex(paramIndex.ParamEyeForm));
			if (paramIndex.ParamMouthOpenY > -1)
				Live2DManager::SetParameterBinding(paramIndex.ParamMouthOpenY, ParameterGui::GetPtrValueByIndex(paramIndex.ParamMouthOpenY));
			if (paramIndex.ParamMouthForm > -1)
				Live2DManager::SetParameterBinding(paramIndex.ParamMouthForm, ParameterGui::GetPtrValueByIndex(paramIndex.ParamMouthForm));
			if (paramIndex.ParamBrowLY > -1)
				Live2DManager::SetParameterBinding(paramIndex.ParamBrowLY, ParameterGui::GetPtrValueByIndex(paramIndex.ParamBrowLY));
			if (paramIndex.ParamBrowRY > -1)
				Live2DManager::SetParameterBinding(paramIndex.ParamBrowRY, ParameterGui::GetPtrValueByIndex(paramIndex.ParamBrowRY));
			if (paramIndex.ParamBrowLForm > -1)
				Live2DManager::SetParameterBinding(paramIndex.ParamBrowLForm, ParameterGui::GetPtrValueByIndex(paramIndex.ParamBrowLForm));
			if (paramIndex.ParamBrowRForm > -1)
				Live2DManager::SetParameterBinding(paramIndex.ParamBrowRForm, ParameterGui::GetPtrValueByIndex(paramIndex.ParamBrowRForm));
			if (paramIndex.ParamBrowLAngle > -1)
				Live2DManager::SetParameterBinding(paramIndex.ParamBrowLAngle, ParameterGui::GetPtrValueByIndex(paramIndex.ParamBrowLAngle));
			if (paramIndex.ParamBrowRAngle > -1)
				Live2DManager::SetParameterBinding(paramIndex.ParamBrowRAngle, ParameterGui::GetPtrValueByIndex(paramIndex.ParamBrowRAngle));

		}

	public:
		inline static struct Live2DManager::ParameterValue OptimizedParameter = {};

	private:
		inline static Ioface s_Ioface = Ioface(); // initialize directly
	};
}