#pragma once

namespace Iolive {
	class IoliveGui
	{
	public:
		static void Init();
		static void Shutdown();
		static void OnDraw();

	private:
		static void SetupGuiTheme();
		static void DrawAllWidget();
	};
} // namespace Iolive