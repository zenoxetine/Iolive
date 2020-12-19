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

		static void GetWindowSize(int* outWidth, int* outHeight) { glfwGetWindowSize(s_Window, outWidth, outHeight); }
		static GLFWwindow* GetWindow() { return s_Window; }

	private:
		inline static GLFWwindow* s_Window;
	};

} // namespace Iolive