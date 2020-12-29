#include "Application.hpp"
#include <GL/glew.h>

// many of these, are a static class method
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
		auto _stackElapsed = StackLogger([](float elapsed_ms) {
			MainWidget::LogScene.AddLogf("[Iolive][I] App initialization passed: %.fms\n", elapsed_ms); }
		);

		Window::Create(WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT);
		Window::SetFrameResizedCallback(&Application::OnFrameResizedCallback);
		Window::SetScrollCallback(&Application::OnScrollCallback);
		Window::SetCursorPosCallback(&Application::OnCursorPosCallback);

		IoliveGui::Init();

		Live2DManager::SetLogFunction(&(MainWidget::LogScene.AddLogf));
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

		MainWidget::LogScene.AddLog("[Iolive][I] App running ...\n");

		// Application loop
		while (!Window::PollEvents())
		{
			OnUpdate();
			OnRender();
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
			MainWidget::LogScene.AddLog("[Iolive][I] Successfully opened the camera\n");

			// make separate thread for face capture loop
			flags_StopCapture = false;
			faceCaptureThread = std::thread(&Application::FaceCaptureLoop, this);
			MainWidget::LogScene.AddLog("[Iolive][I] Face capture thread created\n");

			if (Live2DManager::IsModelInitialized())
			{
				// bind model parameters with OptimizedParameter from IofaceBridge
				IofaceBridge::BindDefaultParametersWithFace();
			}
			return true;
		}
		else
		{
			MainWidget::LogScene.AddLog("[Iolive][E] Can't open the camera\n");
			return false;
		}
	}

	void Application::CloseCamera()
	{
		// tell faceCaptureThread to break the loop
		MainWidget::LogScene.AddLog("[Iolive][I] Stopping face capture thread\n");
		flags_StopCapture = true;
		if (faceCaptureThread.joinable())
			faceCaptureThread.join();
		
		IofaceBridge::CloseCamera();
		MainWidget::LogScene.AddLog("[Iolive][I] Camera closed\n");

		if (Live2DManager::IsModelInitialized())
		{
			// bind model parameters with the gui
			IofaceBridge::BindDefaultParametersWithGui();
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
			float scale = yoffset / 12;

			float nextModelScale = Live2DManager::GetModelScale() + scale;
			if (nextModelScale >= 0.05f)
				Live2DManager::SetModelScale(nextModelScale);
			else
				Live2DManager::SetModelScale(0.05f);
		}
	}

	void Application::OnCursorPosCallback(bool pressed, double xpos, double ypos)
	{
		static bool hasReleased = true;
		static double lastX = -1.0;
		static double lastY = -1.0;

		if (!pressed)
		{
			hasReleased = true; // mouse released || not clicked
		}

		if (lastX >= 0.0 && lastY >= 0.0)
		{
			if (!hasReleased && pressed && xpos > 0.0 && ypos > 0.0) // allow dragging on window screen only
			{
				if (Live2DManager::IsModelInitialized())
				{
					double xDist = xpos - lastX;
					double yDist = ypos - lastY;
					lastX = xpos;
					lastY = ypos;

					int wWidth, wHeight;
					Window::GetWindowSize(&wWidth, &wHeight);

					xDist = MathUtils::Normalize(xDist, 0.f, wWidth / 2);
					yDist = MathUtils::Normalize(yDist, 0.f, wHeight / 2);

					Live2DManager::AddModelTranslateX(xDist);
					Live2DManager::AddModelTranslateY(-yDist);
				}
			}
		}
		else
		{
			if (pressed) // first time mouse clicked
			{
				lastX = xpos;
				lastY = ypos;
			}
		}

		if (pressed)
		{
			// start draging mouse in the next frame (if mouse isn't released)
			hasReleased = false;
			lastX = xpos;
			lastY = ypos;
		}
	}
} // namespace Iolive