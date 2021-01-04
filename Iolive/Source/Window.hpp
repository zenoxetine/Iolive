#pragma once

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <mutex>

namespace Iolive {
	class Window {
	private:
		inline static Window* s_Window = nullptr; // static window instance
		inline static std::mutex s_MtxGetInstance;
	public:
		void Destroy();

		static Window* Create(const char* title, int width, int height);
		static Window* Get();

		/*
		* poll window events
		* \return bool isWindowShouldClose?
		*/
		bool PollEvents();
		void SwapWindow();

		void SetWindowOpacity(float value);

		GLFWwindow* GetGlfwWindow();
		void GetWindowSize(int* outWidth, int* outHeight);
		double GetDeltaTime() const;

	private:
		Window(const char* title, int width, int height);

	public:
		// callback without GLFWwindow
		void(*OnFrameResizedCallback)(int width, int height) = nullptr;
		void(*OnScrollCallback)(double xoffset, double yoffset) = nullptr;
		void(*OnCursorPosCallback)(bool pressed, double xpos, double ypos) = nullptr;

		float MaxFPS = 60.0f;

	private:
		GLFWwindow* m_GlfwWindow = nullptr;

		double m_CurrentFrame = 0.0;
		double m_LastFrame = 0.0;
		double m_DeltaTime = 0.02; // in seconds
	};
} // namespace Iolive