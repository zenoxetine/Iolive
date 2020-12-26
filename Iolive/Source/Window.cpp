#include "Window.hpp"
#include "Logger.hpp"
#include <stdexcept>
#include <thread>
#include <chrono>

namespace Iolive {
	void Window::Create(const char* title, int width, int height)
	{
		auto _stackElapsed = StackLogger("Window Creation");

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

		glfwSetFramebufferSizeCallback(s_Window, PreFrameResizeCallback);
		glfwSetScrollCallback(s_Window, PreScrollCallback);
	}

	void Window::PreFrameResizeCallback(GLFWwindow* window, int width, int height)
	{
		if (s_DoFrameResizedCallback != nullptr)
			s_DoFrameResizedCallback(width, height);
	}

	void Window::PreScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
	{
		if (s_DoScrollCallback != nullptr)
			s_DoScrollCallback(xoffset, yoffset);
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