#pragma once

#include "Widget/MainLWidget.hpp"
#include "Widget/MainRWidget.hpp"

namespace Iolive {
	class IoliveGui
	{
	public:
		static void Init();
		static void Shutdown();
		static void OnDraw();

	private:
		static void InitWidget();
		static void DrawAllWidget();

	public:
		inline static MainLeftWidget LeftWidget;
		inline static MainRightWidget RightWidget;
	};
} // namespace Iolive