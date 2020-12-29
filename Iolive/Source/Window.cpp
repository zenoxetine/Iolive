#include "Window.hpp"
#include <stdexcept>
#include <thread>
#include <chrono>

namespace Iolive {
	void Window::Create(const char* title, int width, int height)
	{
		glfwInit();

		s_Window = glfwCreateWindow(width, height, title, NULL, NULL);
		SetWindowVisible(false);

		// create window context first
		glfwMakeContextCurrent(s_Window);

		// then initialize glew
		if (glewInit() != GLEW_OK)
		{
			throw std::runtime_error("Can't initialize opengl loader");
		}

		glfwSetFramebufferSizeCallback(s_Window, FrameResizedCallback);
		glfwSetScrollCallback(s_Window, ScrollCallback);
		glfwSetCursorPosCallback(s_Window, CursorPosCallback);
	}

	void Window::FrameResizedCallback(GLFWwindow* window, int width, int height)
	{
		if (s_OnFrameResizedCallback != nullptr)
			s_OnFrameResizedCallback(width, height);
	}

	void Window::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
	{
		if (s_OnScrollCallback != nullptr)
			s_OnScrollCallback(xoffset, yoffset);
	}

	void Window::CursorPosCallback(GLFWwindow* window, double xpos, double ypos)
	{
		if (s_OnCursorPosCallback == nullptr) return;

		int lMouseButtonState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
	
		if (lMouseButtonState == GLFW_PRESS)
			s_OnCursorPosCallback(true, xpos, ypos); // pressed: true
		else if (lMouseButtonState == GLFW_RELEASE)
			s_OnCursorPosCallback(false, xpos, ypos); // pressed: false
	}

	void Window::Destroy()
	{
		glfwDestroyWindow(s_Window);
		glfwTerminate();
	}

	bool Window::PollEvents()
	{
		glfwPollEvents();
		return glfwWindowShouldClose(s_Window);
	}

	void Window::SwapWindow()
	{
		// cap fps
		long msToSleep = (1000.0 / s_MaxFPS) - (s_DeltaTime * 100.0);
		if (msToSleep > 0.0)
			std::this_thread::sleep_for(std::chrono::milliseconds(msToSleep));

		// update delta time
		UpdateDeltaTime();

		// swap window buffers
		glfwSwapBuffers(s_Window);
	}

	void Window::SetWindowVisible(bool visible)
	{
		glfwSetWindowAttrib(s_Window, GLFW_VISIBLE, visible);
	}

	void Window::UpdateDeltaTime()
	{
		// delta time in seconds
		s_CurrentFrame = glfwGetTime();
		s_DeltaTime = s_CurrentFrame - s_LastFrame;
		s_LastFrame = s_CurrentFrame;
	}

} // namespace Iolive