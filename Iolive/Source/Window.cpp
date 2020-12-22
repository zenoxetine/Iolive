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

		// glfwSetFramebufferSizeCallback(m_Window, FramebufferSizeCallback);
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