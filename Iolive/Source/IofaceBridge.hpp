#pragma once

#include "Ioface/Ioface.hpp"
#include "Window.hpp"
#include "MathUtils.hpp"
#include "GUI/IoliveGui.hpp"

namespace Iolive {
	class IofaceBridge
	{
	public:
		// static Ioface* Get() { return &s_Ioface; }

		static void UpdateIoface()
		{
			s_Ioface.UpdateAll();

			/*
			* to avoid bugs, the "show frame" feature is now disabled
			if (IoliveGui::LeftWidget.GetCheckboxShowFrame().IsChanged())
			{
				if (IoliveGui::LeftWidget.GetCheckboxShowFrame().IsChecked() == false)
					s_Ioface.CloseAllFrame(); // destroy all frame
			}

			if (IoliveGui::LeftWidget.GetCheckboxShowFrame().IsChecked())
				s_Ioface.ShowFrame(); // show frame
			*/
		}

		static void DoOptimizeParameters()
		{
			if (s_Ioface.IsCameraOpened())
			{
				// float deltaTime = static_cast<float>(Window::GetDeltaTime());

				Parameters.AngleX = MathUtils::Lerp(Parameters.AngleX, s_Ioface.AngleX, 0.06f);
				Parameters.AngleY = MathUtils::Lerp(Parameters.AngleY, s_Ioface.AngleY, 0.06f);
				Parameters.AngleZ = MathUtils::Lerp(Parameters.AngleZ, s_Ioface.AngleZ, 0.06f);

				// mouth open y
				Parameters.MouthOpenY = s_Ioface.MouthOpenY / 20.f;
				if (Parameters.MouthOpenY < 0.3f)
					Parameters.MouthOpenY = 0.0f;
				else if (Parameters.MouthOpenY > 0.3f && Parameters.MouthOpenY < 0.7f)
					Parameters.MouthOpenY *= 1.5f;
				else if (Parameters.MouthOpenY > 0.9f)
					Parameters.MouthOpenY = 1.0f;

				// mouth form
				Parameters.MouthForm = s_Ioface.MouthForm / 90.f;
			}
		}

		static bool OpenCamera(int deviceId) { return s_Ioface.OpenCamera(deviceId); }
		static void CloseCamera() { s_Ioface.CloseCamera(); }

		static bool IsCameraOpened() { return s_Ioface.IsCameraOpened(); }

	public:
		inline static struct OptimizedParameters
		{
			float AngleX = 0.f;
			float AngleY = 0.f;
			float AngleZ = 0.f;
			float EyeLOpen = 1.0f;
			float EyeROpen = 1.0f;
			float MouthOpenY = 0.0f;
			float MouthForm = 1.0f;
		} Parameters;

	private:
		inline static Ioface s_Ioface = Ioface(); // initialize directly
	};
}