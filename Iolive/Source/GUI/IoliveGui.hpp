#pragma once

namespace Iolive {
	class IoliveGui
	{
	public:
		IoliveGui() = delete;

		static void Init();
		static void Shutdown();
		static void OnDraw();

	private:
		static void SetupGuiTheme();
		static void DrawAllWidget();
	};
} // namespace Iolive