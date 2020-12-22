#include "Application.hpp"
#include <GL/glew.h>
#include "Window.hpp"
#include "IofaceBridge.hpp"
#include "GUI/IoliveGui.hpp"
#include "Live2D/Live2DManager.hpp"

#define WINDOW_WIDTH 555  // HAHAHA
#define WINDOW_HEIGHT 666 // HIIII
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

		// Application loop
		while (!Window::PollEvents())
		{
			OnUpdate();
			OnRender();
		}
	}

	void Application::OnUpdate()
	{
		if (IoliveGui::LeftWidget.GetCheckboxFaceCapture().IsChanged())
		{
			if (IoliveGui::LeftWidget.GetCheckboxFaceCapture().IsChecked())
			{
				if (!OpenCamera())
				{
					IoliveGui::LeftWidget.GetCheckboxFaceCapture().SetChecked(false);
				}
			}
			else
			{
				CloseCamera();
			}
		}

		IofaceBridge::DoOptimizeParameters();

		if (Live2DManager::IsModelChanged() && IofaceBridge::IsCameraOpened())
		{
			// there's a new model and camera opened
			// but model parameter wasn't binded with face capture. Bind it now
			BindDefaultParametersWithFace();
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
			IofaceBridge::UpdateIoface(); // capturing new frame from camera
		}
	}

	bool Application::OpenCamera()
	{
		if (IofaceBridge::OpenCamera(0))
		{
			// create separate thread for face capture loop
			flags_StopCapture = false;
			faceCaptureThread = std::thread(&Application::FaceCaptureLoop, this);

			// if model was opened,
			// bind those default parameter with pointer of optimized parameter from IofaceBridge
			if (Live2DManager::IsModelInitialized())
			{
				BindDefaultParametersWithFace();
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

		// close camera
		IofaceBridge::CloseCamera();

		if (Live2DManager::IsModelInitialized())
		{
			// bind parameters with the gui
			BindDefaultParametersWithGui();
		}
	}

	void Application::BindDefaultParametersWithFace()
	{
		const Live2DManager::DefaultParameter& paramIndex = Live2DManager::IndexOfDefaultParameter;
		if (paramIndex.ParamAngleX > -1)
			Live2DManager::SetParameterBinding(paramIndex.ParamAngleX, &(IofaceBridge::Parameters.AngleX));
		if (paramIndex.ParamAngleY > -1)
			Live2DManager::SetParameterBinding(paramIndex.ParamAngleY, &(IofaceBridge::Parameters.AngleY));
		if (paramIndex.ParamAngleZ > -1)
			Live2DManager::SetParameterBinding(paramIndex.ParamAngleZ, &(IofaceBridge::Parameters.AngleZ));
		if (paramIndex.ParamEyeLOpen > -1)
			Live2DManager::SetParameterBinding(paramIndex.ParamEyeLOpen, &(IofaceBridge::Parameters.EyeLOpen));
		if (paramIndex.ParamEyeROpen > -1)
			Live2DManager::SetParameterBinding(paramIndex.ParamEyeROpen, &(IofaceBridge::Parameters.EyeROpen));
		if (paramIndex.ParamMouthOpenY > -1)
			Live2DManager::SetParameterBinding(paramIndex.ParamMouthOpenY, &(IofaceBridge::Parameters.MouthOpenY));
		if (paramIndex.ParamMouthForm > -1)
			Live2DManager::SetParameterBinding(paramIndex.ParamMouthForm, &(IofaceBridge::Parameters.MouthForm));
	}

	void Application::BindDefaultParametersWithGui()
	{
		const Live2DManager::DefaultParameter& paramIndex = Live2DManager::IndexOfDefaultParameter;

		if (paramIndex.ParamAngleX > -1)
		{
			float* ptrGuiParamAngleX = IoliveGui::LeftWidget.GetParameterGui().GetPtrValueByIndex(paramIndex.ParamAngleX);
			Live2DManager::SetParameterBinding(paramIndex.ParamAngleX, ptrGuiParamAngleX);
		}

		if (paramIndex.ParamAngleY > -1)
		{
			float* ptrGuiParamAngleY = IoliveGui::LeftWidget.GetParameterGui().GetPtrValueByIndex(paramIndex.ParamAngleY);
			Live2DManager::SetParameterBinding(paramIndex.ParamAngleY, ptrGuiParamAngleY);
		}

		if (paramIndex.ParamAngleZ > -1)
		{
			float* ptrGuiParamAngleZ = IoliveGui::LeftWidget.GetParameterGui().GetPtrValueByIndex(paramIndex.ParamAngleZ);
			Live2DManager::SetParameterBinding(paramIndex.ParamAngleZ, ptrGuiParamAngleZ);
		}
		
		if (paramIndex.ParamEyeLOpen > -1)
		{
			float* ptrGuiParamEyeLOpen = IoliveGui::LeftWidget.GetParameterGui().GetPtrValueByIndex(paramIndex.ParamEyeLOpen);
			Live2DManager::SetParameterBinding(paramIndex.ParamEyeLOpen, ptrGuiParamEyeLOpen);
		}
		
		if (paramIndex.ParamEyeROpen > -1)
		{
			float* ptrGuiParamEyeROpen = IoliveGui::LeftWidget.GetParameterGui().GetPtrValueByIndex(paramIndex.ParamEyeROpen);
			Live2DManager::SetParameterBinding(paramIndex.ParamEyeROpen, ptrGuiParamEyeROpen);
		}
		
		if (paramIndex.ParamMouthOpenY > -1)
		{
			float* ptrGuiParamMouthOpenY = IoliveGui::LeftWidget.GetParameterGui().GetPtrValueByIndex(paramIndex.ParamMouthOpenY);
			Live2DManager::SetParameterBinding(paramIndex.ParamMouthOpenY, ptrGuiParamMouthOpenY);
		}
		
		if (paramIndex.ParamMouthForm > -1)
		{
			float* ptrGuiParamMouthForm = IoliveGui::LeftWidget.GetParameterGui().GetPtrValueByIndex(paramIndex.ParamMouthForm);
			Live2DManager::SetParameterBinding(paramIndex.ParamMouthForm, ptrGuiParamMouthForm);
		}
	}
}