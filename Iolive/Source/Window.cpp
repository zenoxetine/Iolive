#include "Window.hpp"
#include <stdexcept>
#include <thread>
#include <chrono>
#include <iostream>

namespace Iolive {
	Window* Window::Create(const char* title, int width, int height)
	{
		if (!s_Window)
			s_Window = new Window(title, width, height);
		
		return s_Window;
	}

	Window* Window::Get()
	{
		std::lock_guard<std::mutex> lock(s_MtxGetInstance);
		return s_Window;
	}

	Window::Window(const char* title, int width, int height)
	{
		if (!glfwInit()) throw std::runtime_error("Can't initialize GLFW!");

		m_GlfwWindow = glfwCreateWindow(width, height, title, NULL, NULL);
		if (!m_GlfwWindow) throw std::runtime_error("Can't create window!");

		glfwSetWindowUserPointer(m_GlfwWindow, this);

		// create window context first
		glfwMakeContextCurrent(m_GlfwWindow);
		
		// then initialize glew
		if (glewInit() != GLEW_OK)
			throw std::runtime_error("Can't initialize opengl loader");

		/*
		* Window callback
		*/
		glfwSetFramebufferSizeCallback(m_GlfwWindow, [](GLFWwindow* window, int width, int height) {
			Window* thisWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));
			if (!(thisWindow->OnFrameResizedCallback)) return;

			thisWindow->OnFrameResizedCallback(width, height);
		});

		glfwSetScrollCallback(m_GlfwWindow, [](GLFWwindow* window, double xoffset, double yoffset) {
			Window* thisWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));
			if (!(thisWindow->OnScrollCallback)) return;

			thisWindow->OnScrollCallback(xoffset, yoffset);
		});

		glfwSetCursorPosCallback(m_GlfwWindow, [](GLFWwindow* window, double xpos, double ypos) {
			Window* thisWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));
			if (!(thisWindow->OnCursorPosCallback)) return;
			
			int LMouseButtonState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
			if (LMouseButtonState == GLFW_PRESS)
				thisWindow->OnCursorPosCallback(true, xpos, ypos); // pressed: true
			else if (LMouseButtonState == GLFW_RELEASE)
				thisWindow->OnCursorPosCallback(false, xpos, ypos); // pressed: false
		});
	}

	void Window::Destroy()
	{
		glfwDestroyWindow(m_GlfwWindow);
		glfwTerminate();
	}

	bool Window::PollEvents()
	{
		glfwPollEvents();
		return glfwWindowShouldClose(m_GlfwWindow);
	}

	void Window::SwapWindow()
	{
		// update delta time
		m_CurrentFrame = glfwGetTime();
		m_DeltaTime = m_CurrentFrame - m_LastFrame;
		m_LastFrame = m_CurrentFrame;

		// cap fps
		long msToSleep = (1000.0 / MaxFPS) - (m_DeltaTime * 100.0);
		if (msToSleep > 0.0)
			std::this_thread::sleep_for(std::chrono::milliseconds(msToSleep - 1));

		// swap window buffers
		glfwSwapBuffers(m_GlfwWindow);
	}

	/* * * * * * * * *
	* Getter & Setter
	*/

	GLFWwindow* Window::GetGlfwWindow()
	{
		return m_GlfwWindow;
	}

	void Window::SetWindowOpacity(float value)
	{
		glfwSetWindowOpacity(m_GlfwWindow, value);
	}

	void Window::GetWindowSize(int* outWidth, int* outHeight)
	{
		glfwGetWindowSize(m_GlfwWindow, outWidth, outHeight);
	}

	double Window::GetDeltaTime() const
	{
		return m_DeltaTime;
	}

} // namespace Iolive