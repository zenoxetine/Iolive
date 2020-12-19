#include "Application.hpp"
#include <GL/glew.h>
#include "Window.hpp"
#include "GUI/IoliveGui.hpp"
#include "Live2D/Live2DManager.hpp"

#define	WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define WINDOW_TITLE "Iolive"

namespace Iolive {
	Application::Application()
	  : m_Ioface(Ioface())
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

		CloseCamera();

		// Application loop
		while (!Window::PollEvents())
		{
			OnUpdate();
			OnRender();
		}
	}

	void Application::OnUpdate()
	{
		m_Ioface.UpdateParameters();

		if (IoliveGui::LeftWidget.CameraCheckboxChanged())
		{
			if (IoliveGui::LeftWidget.IsCameraChecked())
			{
				if (!OpenCamera())
				{
					IoliveGui::LeftWidget.SetCameraChecked(false);
				}
			}
			else
			{
				CloseCamera();
			}
		}

		if (Live2DManager::IsModelChanged() /*&& Live2DManager::IsModelInitialized()*/ && m_Ioface.IsCameraOpened())
		{
			// there's new a model and camera opened
			// but face capture wasn't binded with model parameter. Bind it now
			BindDefaultParametersWithFace();
		}

		Live2DManager::OnUpdate();
	}
	
	void Application::OnRender()
	{
		int width, height;
		Window::GetWindowSize(&width, &height);

		glViewport(0, 0, width, height);
		glClearColor(0.2, 0.9f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		m_Ioface.ShowFrame();

		Live2DManager::OnDraw(width, height);

		IoliveGui::OnDraw();
		
		Window::SwapWindow();
	}

	bool Application::OpenCamera()
	{
		if (m_Ioface.OpenCamera(0))
		{
			if (Live2DManager::IsModelInitialized())
			{
				BindDefaultParametersWithFace();
			}
		}

		return m_Ioface.IsCameraOpened();
	}

	void Application::CloseCamera()
	{
		m_Ioface.CloseCamera();

		if (Live2DManager::IsModelInitialized())
		{
			BindDefaultParametersWithGui();
		}
	}

	void Application::BindDefaultParametersWithFace()
	{
		const Live2DManager::DefaultParameter& paramIndex = Live2DManager::IndexOfDefaultParameter;
		if (paramIndex.ParamAngleX > -1)
			Live2DManager::SetParameterBinding(paramIndex.ParamAngleX, &(m_Ioface.angleX));
		if (paramIndex.ParamAngleY > -1)
			Live2DManager::SetParameterBinding(paramIndex.ParamAngleY, &(m_Ioface.angleY));
		if (paramIndex.ParamAngleZ > -1)
			Live2DManager::SetParameterBinding(paramIndex.ParamAngleZ, &(m_Ioface.angleZ));
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
	}
}