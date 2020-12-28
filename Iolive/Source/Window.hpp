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
		
		/*
		* Set callback function
		*/
		static void SetFrameResizedCallback(void(*callback)(int, int)) { s_OnFrameResizedCallback = callback; }
		static void SetScrollCallback(void(*callback)(double, double)) { s_OnScrollCallback = callback; }
		static void SetCursorPosCallback(void(*callback)(bool, double, double)) { s_OnCursorPosCallback = callback; }

		static GLFWwindow* GetWindow() { return s_Window; }
		static void GetWindowSize(int* outWidth, int* outHeight) { glfwGetWindowSize(s_Window, outWidth, outHeight); }
		static double GetDeltaTime() { return s_DeltaTime; }

	private:
		// callback from glfw
		static void FrameResizedCallback(GLFWwindow* window, int width, int height);
		static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
		static void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);

	private:
		inline static GLFWwindow* s_Window;

		inline static float s_MaxFPS = 60.f;

		inline static double s_CurrentFrame = 0.0;
		inline static double s_LastFrame = 0.0;
		inline static double s_DeltaTime = 0.02; // in seconds

		// callback
		inline static void(*s_OnFrameResizedCallback)(int width, int height) = nullptr;
		inline static void(*s_OnScrollCallback)(double xoffset, double yoffset) = nullptr;
		inline static void(*s_OnCursorPosCallback)(bool pressed, double xpos, double ypos) = nullptr;
	};

} // namespace Iolive