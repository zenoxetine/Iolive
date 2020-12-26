#include "Application.hpp"
#include <GL/glew.h>

// many of these, are a static class function
#include "GUI/IoliveGui.hpp"
#include "GUI/Widget/MainWidget.hpp"
#include "GUI/Widget/ParameterGui.hpp"
#include "Window.hpp"
#include "IofaceBridge.hpp"
#include "Live2D/Live2DManager.hpp"

#include "Logger.hpp"
#include "MathUtils.hpp"

#define WINDOW_WIDTH 540
#define WINDOW_HEIGHT 670
#define WINDOW_TITLE "Iolive"

namespace Iolive {
	Application::Application()
	  :	flags_StopCapture(true)
	{
		Window::Create(WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT);
		IoliveGui::Init();
		Live2DManager::Init();
	}

	Application::~Application()
	{
		CloseCamera();
		Live2DManager::Release();
		IoliveGui::Shutdown();
		Window::Destroy();
	}

	void Application::Run()
	{
		Window::SetWindowVisible(true);
		Window::SetFrameResizedCallback(&Application::OnFrameResizedCallback);
		Window::SetScrollCallback(&Application::OnScrollCallback);

		// Application loop
		while (!Window::PollEvents())
		{
			OnUpdate();
			OnRender();
		}
	}

	void Application::OnFrameResizedCallback(int width, int height)
	{
		Application::Get()->OnRender();
	}

	void Application::OnScrollCallback(double xoffset, double yoffset)
	{
		if (Live2DManager::IsModelInitialized())
		{
			// float scale = MathUtils::Lerp(yoffset * 0.02f, yoffset, 0.05f);
			float scale = yoffset / 7;

			float nextModelScale = Live2DManager::GetModelScale() + scale;
			if (nextModelScale >= 0.0f)
				Live2DManager::SetModelScale(nextModelScale);
			else
				Live2DManager::SetModelScale(0.0f);
		}
	}

	void Application::OnUpdate()
	{
		if (MainWidget::GetCheckbox_FaceCapture().IsChanged())
		{
			if (MainWidget::GetCheckbox_FaceCapture().IsChecked())
			{
				if (!OpenCamera())
				{
					MainWidget::GetCheckbox_FaceCapture().SetChecked(false);
				}
			}
			else
			{
				CloseCamera();
			}
		}

		IofaceBridge::DoOptimizeParameters();

		if (Live2DManager::IsModelChanged())
		{
			if (IofaceBridge::IsCameraOpened())
			{
				// there's a new model and camera opened
				// but model parameter wasn't binded with face capture. Bind it now
				IofaceBridge::BindDefaultParametersWithFace();
			}
		}

		Live2DManager::OnUpdate(Window::GetDeltaTime());
	}
	
	void Application::OnRender()
	{
		int width, height;
		Window::GetWindowSize(&width, &height);

		glViewport(0, 0, width, height);
		glClearColor(0.2, 0.9f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		Live2DManager::OnDraw(width, height);

		IoliveGui::OnDraw();
		
		Window::SwapWindow();
	}

	void Application::FaceCaptureLoop()
	{
		while (!flags_StopCapture)
		{
			IofaceBridge::UpdateIoface(); 
		}
	}

	bool Application::OpenCamera()
	{
		if (IofaceBridge::OpenCamera(0))
		{
			// make separate thread for face capture loop
			flags_StopCapture = false;
			faceCaptureThread = std::thread(&Application::FaceCaptureLoop, this);

			if (Live2DManager::IsModelInitialized())
			{
				// bind model parameters with OptimizedParameter from IofaceBridge
				IofaceBridge::BindDefaultParametersWithFace();
			}
		}

		return IofaceBridge::IsCameraOpened();
	}

	void Application::CloseCamera()
	{
		// tell faceCaptureThread to break the loop
		flags_StopCapture = true;
		if (faceCaptureThread.joinable())
			faceCaptureThread.join();
		
		IofaceBridge::CloseCamera();

		if (Live2DManager::IsModelInitialized())
		{
			// bind model parameters with the gui
			IofaceBridge::BindDefaultParametersWithGui();
		}
	}
}