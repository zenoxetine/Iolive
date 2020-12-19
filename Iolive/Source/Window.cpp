#include "Window.hpp"
#include <stdexcept>

namespace Iolive {
	void Window::Create(const char* title, int width, int height)
	{
		glfwInit();
		s_Window = glfwCreateWindow(width, height, title, NULL, NULL);
		glfwSetWindowCloseCallback(s_Window, [](GLFWwindow* window) {
			glfwSetWindowShouldClose(window, GLFW_TRUE);
		});
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
		glfwSwapBuffers(s_Window);
	}

	void Window::SetWindowVisible(bool visible)
	{
		glfwSetWindowAttrib(s_Window, GLFW_VISIBLE, visible);
	}

} // namespace Iolive