#pragma once

#include "Ioface/Ioface.hpp"
#include "IofaceBridge.hpp"
#include "Live2D/Live2DManager.hpp"
#include "GUI/Widget/MainWidget.hpp"
#include "GUI/Widget/ParameterGui.hpp"
#include "MathUtils.hpp"
#include "Logger.hpp"

#include <windef.h>
#include <winuser.h>

namespace Iolive {

	/*
	* Get the horizontal and vertical screen sizes in pixel
	*/
	void GetDesktopResolution(int* horizontal, int* vertical)
	{
		RECT desktop;
		const HWND hDesktop = GetDesktopWindow();
		GetWindowRect(hDesktop, &desktop);

		*horizontal = desktop.right;
		*vertical = desktop.bottom;
	}

	/*
	* Get mouse x and y position
	*/
	void GetMousePosition(int* x, int* y)
	{
		POINT mousePoint;
		if (GetCursorPos(&mousePoint))
		{
			*x = mousePoint.x;
			*y = mousePoint.y;
		}
	}

	class ParameterBridge
	{
	public:
		ParameterBridge() = delete;
		
		static void DoOptimizeParameters()
		{
			Ioface& s_Ioface = IofaceBridge::Get();
			if (IofaceBridge::IsCameraOpened() && s_Ioface.IsDetected())
			{
				/* 
				* Update Parameters from Ioface
				*/

				float deltaTime = static_cast<float>(Window::GetDeltaTime());
				#define SMOOTH_SLOW(start, end) MathUtils::Lerp(start, end, deltaTime * 5.f)
				#define SMOOTH_MEDIUM(start, end) MathUtils::Lerp(start, end, deltaTime * 10.f)
				#define SMOOTH_FAST(start, end) MathUtils::Lerp(start, end, deltaTime * 15.f)

				// ParamAngle
				OptimizedParameter.ParamAngleX = SMOOTH_SLOW(OptimizedParameter.ParamAngleX, s_Ioface.AngleX);
				OptimizedParameter.ParamAngleY = SMOOTH_SLOW(OptimizedParameter.ParamAngleY, s_Ioface.AngleY * 1.3f);
				OptimizedParameter.ParamAngleZ = SMOOTH_SLOW(OptimizedParameter.ParamAngleZ, s_Ioface.AngleZ);

				// BodyAngle
				OptimizedParameter.ParamBodyAngleX = OptimizedParameter.ParamAngleX * 0.2f;
				OptimizedParameter.ParamBodyAngleY = OptimizedParameter.ParamAngleY * 0.25f;
				OptimizedParameter.ParamBodyAngleZ = OptimizedParameter.ParamAngleZ * 0.2f;

				// MouthOpenY
				float normalizedMouthOpenY = MathUtils::Normalize(s_Ioface.DistScale * s_Ioface.MouthOpenY, 3.5f, 14.0f);
				OptimizedParameter.ParamMouthOpenY = SMOOTH_FAST(OptimizedParameter.ParamMouthOpenY, normalizedMouthOpenY);

				// MouthForm
				float normalizedMouthForm = MathUtils::Normalize(s_Ioface.DistScale * s_Ioface.MouthForm, 71.0f, 85.0f);
				OptimizedParameter.ParamMouthForm = SMOOTH_FAST(OptimizedParameter.ParamMouthForm, normalizedMouthForm);

				if (MainWidget::GetCheckbox_EqualizeEyes().IsChecked())
				{
					// Equalize EyeOpenY Left & Right value
					float normalizedEAR = MathUtils::Normalize(s_Ioface.DistScale * s_Ioface.EAR, s_Ioface.DistScale * 0.13f, s_Ioface.DistScale * 0.23f);
					OptimizedParameter.ParamEyeLOpen = SMOOTH_MEDIUM(OptimizedParameter.ParamEyeLOpen, normalizedEAR);
					OptimizedParameter.ParamEyeROpen = OptimizedParameter.ParamEyeLOpen; // same
				}
				else
				{
					// EyeOpenLY
					float normalizedLeftEAR = MathUtils::Normalize(s_Ioface.DistScale * s_Ioface.LeftEAR, s_Ioface.DistScale * 0.13f, s_Ioface.DistScale * 0.23f);
					OptimizedParameter.ParamEyeLOpen = SMOOTH_MEDIUM(OptimizedParameter.ParamEyeLOpen, normalizedLeftEAR);
					// EyeOpenRY
					float normalizedRightEAR = MathUtils::Normalize(s_Ioface.DistScale * s_Ioface.RightEAR, s_Ioface.DistScale * 0.13f, s_Ioface.DistScale * 0.23f);
					OptimizedParameter.ParamEyeROpen = SMOOTH_MEDIUM(OptimizedParameter.ParamEyeROpen, normalizedRightEAR);
				}

				/*// Eye smile based on MouthForm
				OptimizedParameter.ParamEyeForm = MathUtils::Normalize(OptimizedParameter.ParamMouthForm, -0.3, 0.9f);
				OptimizedParameter.ParamEyeLSmile = OptimizedParameter.ParamEyeForm; // both left | right are equal ^^
				OptimizedParameter.ParamEyeRSmile = OptimizedParameter.ParamEyeForm;*/

				// Eye smile based on AngleY
				float normalizedEyeForm = MathUtils::Normalize(OptimizedParameter.ParamAngleY, -20.0f, 10.0f);
				OptimizedParameter.ParamEyeForm = SMOOTH_MEDIUM(OptimizedParameter.ParamEyeForm, normalizedEyeForm);
				OptimizedParameter.ParamEyeLSmile = OptimizedParameter.ParamEyeForm; // both left | right are equal ^^
				OptimizedParameter.ParamEyeRSmile = OptimizedParameter.ParamEyeForm;

				// EyeBrowY, left & right value will be equal
				float optBrowLY = MathUtils::Normalize(s_Ioface.DistScale * s_Ioface.EyeBrowLY, 45.0f, 53.0f);
				float optBrowRY = MathUtils::Normalize(s_Ioface.DistScale * s_Ioface.EyeBrowRY, 45.0f, 53.0f);
				float avgBrow = (optBrowLY + optBrowRY) / 2.f;
				OptimizedParameter.ParamBrowLY = SMOOTH_SLOW(OptimizedParameter.ParamBrowLY, avgBrow);
				OptimizedParameter.ParamBrowRY = OptimizedParameter.ParamBrowLY; // same

				// EyeBrowForm follow EyeBrowY, but <= 0.0f
				OptimizedParameter.ParamBrowLForm = OptimizedParameter.ParamBrowLY < 0.0f ? OptimizedParameter.ParamBrowLY : 0.0f;
				OptimizedParameter.ParamBrowRForm = OptimizedParameter.ParamBrowLForm;

				// EyeBrowAngle Follow EyeBrowForm
				OptimizedParameter.ParamBrowLAngle = OptimizedParameter.ParamBrowLForm;
				OptimizedParameter.ParamBrowRAngle = OptimizedParameter.ParamBrowLForm;

				// Update Eye Ball X & Y parameters based on cursor position on the screen
				static int screenWidth = 0, screenHeight = 0;
				if (screenWidth == 0 && screenHeight == 0) Iolive::GetDesktopResolution(&screenWidth, &screenHeight); // only once
				if (screenWidth > 0 && screenHeight > 0)
				{
					const int midScreenWidth = screenWidth / 2;
					const int midScreenHeight = screenHeight / 2;
					int mouseX = midScreenWidth;
					int mouseY = midScreenHeight;
					Iolive::GetMousePosition(&mouseX, &mouseY);

					float normalizedMouseX = MathUtils::Normalize(mouseX, midScreenWidth, screenWidth);
					float normalizedMouseY = MathUtils::Normalize(mouseY, midScreenHeight, screenHeight);
					OptimizedParameter.ParamEyeBallX = normalizedMouseX;
					OptimizedParameter.ParamEyeBallY = -normalizedMouseY;
				}
			}
		}

	public:
		/*** Parameter binding ***/
		static void BindDefaultParametersWithFace()
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
			if (paramIndex.ParamEyeBallX > -1)
				Live2DManager::SetParameterBinding(paramIndex.ParamEyeBallX, &(OptimizedParameter.ParamEyeBallX));
			if (paramIndex.ParamEyeBallY > -1)
				Live2DManager::SetParameterBinding(paramIndex.ParamEyeBallY, &(OptimizedParameter.ParamEyeBallY));
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
		static void BindDefaultParametersWithGui()
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
			if (paramIndex.ParamEyeBallX > -1)
				Live2DManager::SetParameterBinding(paramIndex.ParamEyeBallX, ParameterGui::GetPtrValueByIndex(paramIndex.ParamEyeBallX));
			if (paramIndex.ParamEyeBallY > -1)
				Live2DManager::SetParameterBinding(paramIndex.ParamEyeBallY, ParameterGui::GetPtrValueByIndex(paramIndex.ParamEyeBallY));
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
	};
}