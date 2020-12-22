#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace Iolive {
	class Window {
	public:
		static void Create(const char* title, int width, int height);
		static void Destroy();

		//
		// poll window events
		// return bool isWindowShouldClose?
		//
		static bool PollEvents();
		static void SwapWindow();

		static void SetWindowVisible(bool visible);
		static void SetMaxFPS(float maxFPS) { s_MaxFPS = maxFPS; }

		static void GetWindowSize(int* outWidth, int* outHeight) { glfwGetWindowSize(s_Window, outWidth, outHeight); }
		static GLFWwindow* GetWindow() { return s_Window; }

		static void UpdateDeltaTime();
		static double GetDeltaTime() { return s_DeltaTime; }

	private:
		inline static GLFWwindow* s_Window;

		inline static float s_MaxFPS = 60.f;

		inline static double s_CurrentFrame = 0.0;
		inline static double s_LastFrame = 0.0;
		inline static double s_DeltaTime = 0.02; // in seconds
	};

} // namespace Iolive