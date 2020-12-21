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

		static void UpdateFrame()
		{
			s_Ioface.UpdateFrame();

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

		static void Update()
		{
			s_Ioface.UpdateParameters();

			if (s_Ioface.IsCameraOpened())
			{
				Parameters.AngleX = MathUtils::Lerp(Parameters.AngleX, s_Ioface.angleX, 0.2f);
				Parameters.AngleY = MathUtils::Lerp(Parameters.AngleY, s_Ioface.angleY, 0.2f);
				Parameters.AngleZ = MathUtils::Lerp(Parameters.AngleZ, s_Ioface.angleZ, 0.2f);
			}
		}

		static bool OpenCamera(int deviceId) { return s_Ioface.OpenCamera(deviceId); }
		static void CloseCamera() { s_Ioface.CloseCamera(); }

		static bool IsCameraOpened() { return s_Ioface.IsCameraOpened(); }

	public:
		inline static struct OptimizedParameters
		{
			float AngleX = 0;
			float AngleY = 0;
			float AngleZ = 0;
		} Parameters;

	private:
		inline static Ioface s_Ioface = Ioface(); // initialize directly
	};
}