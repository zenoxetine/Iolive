#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace Iolive {
	class Window {
	public:
		static void Create(const char* title, int width, int height);
		static void Destroy();

		/*
		* poll window events
		* \return bool isWindowShouldClose?
		*/
		static bool PollEvents();
		static void SwapWindow();
		static void UpdateDeltaTime();

		static void SetWindowVisible(bool visible);
		static void SetMaxFPS(float maxFPS) { s_MaxFPS = maxFPS; }
		
		static void SetFrameResizedCallback(void(*callback)(int, int)) { s_DoFrameResizedCallback = callback; }
		static void SetScrollCallback(void(*callback)(double, double)) { s_DoScrollCallback = callback; }

		static GLFWwindow* GetWindow() { return s_Window; }
		static void GetWindowSize(int* outWidth, int* outHeight) { glfwGetWindowSize(s_Window, outWidth, outHeight); }
		static double GetDeltaTime() { return s_DeltaTime; }

	private:
		static void PreFrameResizeCallback(GLFWwindow* window, int width, int height);
		static void PreScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

	private:
		inline static GLFWwindow* s_Window;

		inline static float s_MaxFPS = 60.f;

		inline static double s_CurrentFrame = 0.0;
		inline static double s_LastFrame = 0.0;
		inline static double s_DeltaTime = 0.02; // in seconds

		// callback
		inline static void(*s_DoScrollCallback)(double xoffset, double yoffset) = nullptr;
		inline static void(*s_DoFrameResizedCallback)(int width, int height) = nullptr;
	};

} // namespace Iolive