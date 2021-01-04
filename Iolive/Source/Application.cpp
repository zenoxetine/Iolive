#include "Application.hpp"
#include <GL/glew.h>

#include "Live2D/Live2DManager.hpp"
#include "Utility/WindowsAPI.hpp"
#include "Utility/Logger.hpp"
#include "Utility/MathUtils.hpp"

#define WINDOW_WIDTH 560
#define WINDOW_HEIGHT 700
#define WINDOW_TITLE "Iolive"

namespace Iolive {
	Application::Application()
		: m_Window(Window::Create(WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT)),
		m_Ioface(Ioface()),
		flags_StopCapture(true)
	{
		auto _stackElapsed = StackLogger([](float elapsed_ms) {
			ExampleAppLog::AddLogf("[Iolive][I] App initialization passed: %.fms\n", elapsed_ms);
		});

		// Set window callback
		m_Window->OnFrameResizedCallback = &(Application::OnFrameResizedCallback);
		m_Window->OnScrollCallback = &(Application::OnScrollCallback);
		m_Window->OnCursorPosCallback = &(Application::OnCursorPosCallback);

		Live2DManager::LoggingFunction = &(ExampleAppLog::AddLogf);
		Live2DManager::InitCubism();

		MainGui::InitializeImGui(m_Window->GetGlfwWindow());

		m_Ioface.SetLogFunction(&(ExampleAppLog::AddLogf));
		m_Ioface.Init();
	}

	Application::~Application()
	{
		CloseCamera();
		
		Live2DManager::ReleaseCubism();
		
		MainGui::ShutdownImGui();
		
		m_UserModel.DeleteModel();

		m_Window->Destroy();
		delete m_Window;
	}

	void Application::Run()
	{
		ExampleAppLog::AddLog("[Iolive][I] App running ...\n\n");

		// Application loop
		while (!m_Window->PollEvents())
		{
			OnUpdate();
			OnRender();
		}
	}

	void Application::OnUpdate()
	{
		if (m_Ioface.IsDetected())
			DoOptimizeParameters();

		if (m_UserModel.IsModelInitialized())
			m_UserModel.GetModel2D()->OnUpdate(m_Window->GetDeltaTime());
	}
	
	void Application::OnRender()
	{
		int width, height;
		m_Window->GetWindowSize(&width, &height);
		glViewport(0, 0, width, height);

		float* clearColor = MainGui::Get().ColorEdit_ClearColor;
		glClearColor(clearColor[0], clearColor[1], clearColor[2], 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		MainGui::Get().Draw(this);
		
		if (m_UserModel.IsModelInitialized())
			m_UserModel.GetModel2D()->OnDraw(width, height);
		
		m_Window->SwapWindow();
	}

	void Application::FaceCaptureLoop()
	{
		bool frameClosed = true;
		while (!flags_StopCapture)
		{
			m_Ioface.UpdateAll();

			if (MainGui::Get().Checkbox_ShowFrame.IsChecked())
			{
				frameClosed = false;
				m_Ioface.ShowFrame();
			}
			else if(!frameClosed)
			{
				frameClosed = true;
				m_Ioface.CloseAllFrame();
			}
		}
		
		if (!frameClosed)
			m_Ioface.CloseAllFrame();
	}

	bool Application::OpenCamera()
	{
		int selectedCamId = MainGui::Get().SelectedCameraId;
		ExampleAppLog::AddLogf("[Iolive][I] Opening camera with id: %d\n", selectedCamId);
		if (m_Ioface.OpenCamera(selectedCamId))
		{
			ExampleAppLog::AddLog("[Iolive][I] Successfully opened the camera\n");

			// create separate thread for face capture loop
			flags_StopCapture = false;
			faceCaptureThread = std::thread(&Application::FaceCaptureLoop, this);
			ExampleAppLog::AddLog("[Iolive][I] Face capture thread created\n\n");

			if (m_UserModel.IsModelInitialized())
			{
				// bind model parameters with Ioface
				BindDefaultParametersWithFace();
			}
			return true;
		}
		else
		{
			ExampleAppLog::AddLog("[Iolive][E] Error while opening the camera\n\n");
			return false;
		}
	}
	
	void Application::CloseCamera()
	{
		// tell faceCaptureThread to break the loop
		ExampleAppLog::AddLog("[Iolive][I] Stopping face capture thread\n");
		flags_StopCapture = true;
		if (faceCaptureThread.joinable())
			faceCaptureThread.join();
		
		m_Ioface.CloseCamera();
		ExampleAppLog::AddLog("[Iolive][I] Camera closed\n\n");

		if (m_UserModel.IsModelInitialized())
		{
			// bind model parameters with the gui
			BindDefaultParametersWithGui();
		}
	}

	void Application::OnFrameResizedCallback(int width, int height)
	{
		Application::Get()->OnRender();
	}

	void Application::OnScrollCallback(double xoffset, double yoffset)
	{
		Application* app = Application::Get();
		if (app->m_UserModel.IsModelInitialized())
		{
			float scale = yoffset / 12;

			Model2D* model = app->m_UserModel.GetModel2D();

			float nextModelScale = model->GetModelScale() + scale;
			if (nextModelScale >= 0.0f)
				model->SetModelScale(nextModelScale);
			else
				model->SetModelScale(0.0f);
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
				Application* app = Application::Get();
				if (app->m_UserModel.IsModelInitialized())
				{
					double xDist = xpos - lastX;
					double yDist = ypos - lastY;
					lastX = xpos;
					lastY = ypos;

					int wWidth, wHeight;
					Window::Get()->GetWindowSize(&wWidth, &wHeight);

					xDist = MathUtils::Normalize(xDist, 0.f, wWidth / 2);
					yDist = MathUtils::Normalize(yDist, 0.f, wHeight / 2);

					app->m_UserModel.GetModel2D()->AddModelTranslateX(xDist);
					app->m_UserModel.GetModel2D()->AddModelTranslateY(-yDist);
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

	void Application::DoOptimizeParameters()
	{
		/* Update Parameters from Ioface */

		float deltaTime = static_cast<float>(m_Window->GetDeltaTime());
		#define SMOOTH_SLOW(start, end) MathUtils::Lerp(start, end, deltaTime * 5.f)
		#define SMOOTH_MEDIUM(start, end) MathUtils::Lerp(start, end, deltaTime * 10.f)
		#define SMOOTH_FAST(start, end) MathUtils::Lerp(start, end, deltaTime * 15.f)

		// ParamAngle
		OptimizedParameter.ParamAngleX = SMOOTH_SLOW(OptimizedParameter.ParamAngleX, m_Ioface.AngleX);
		OptimizedParameter.ParamAngleY = SMOOTH_SLOW(OptimizedParameter.ParamAngleY, m_Ioface.AngleY * 1.3f);
		OptimizedParameter.ParamAngleZ = SMOOTH_SLOW(OptimizedParameter.ParamAngleZ, m_Ioface.AngleZ);

		// BodyAngle
		OptimizedParameter.ParamBodyAngleX = OptimizedParameter.ParamAngleX * 0.2f;
		OptimizedParameter.ParamBodyAngleY = OptimizedParameter.ParamAngleY * 0.25f;
		OptimizedParameter.ParamBodyAngleZ = OptimizedParameter.ParamAngleZ * 0.2f;

		// MouthOpenY
		float normalizedMouthOpenY = MathUtils::Normalize(m_Ioface.DistScale * m_Ioface.MouthOpenY, 3.0f, 15.0f);
		OptimizedParameter.ParamMouthOpenY = SMOOTH_FAST(OptimizedParameter.ParamMouthOpenY, normalizedMouthOpenY);

		// MouthForm
		float normalizedMouthForm = MathUtils::Normalize(m_Ioface.DistScale * m_Ioface.MouthForm, 72.0f, 85.0f);
		OptimizedParameter.ParamMouthForm = SMOOTH_MEDIUM(OptimizedParameter.ParamMouthForm, normalizedMouthForm);

		if (MainGui::Get().Checkbox_EqualizeEyes.IsChecked())
		{
			// Equalize EyeOpenY Left & Right value
			float normalizedEAR = MathUtils::Normalize(m_Ioface.DistScale * m_Ioface.EAR, m_Ioface.DistScale * 0.13f, m_Ioface.DistScale * 0.24f);
			OptimizedParameter.ParamEyeLOpen = SMOOTH_MEDIUM(OptimizedParameter.ParamEyeLOpen, normalizedEAR);
			OptimizedParameter.ParamEyeROpen = OptimizedParameter.ParamEyeLOpen; // same
		}
		else
		{
			// EyeOpenLY
			float normalizedLeftEAR = MathUtils::Normalize(m_Ioface.DistScale * m_Ioface.LeftEAR, m_Ioface.DistScale * 0.13f, m_Ioface.DistScale * 0.24f);
			OptimizedParameter.ParamEyeLOpen = SMOOTH_MEDIUM(OptimizedParameter.ParamEyeLOpen, normalizedLeftEAR);
			// EyeOpenRY
			float normalizedRightEAR = MathUtils::Normalize(m_Ioface.DistScale * m_Ioface.RightEAR, m_Ioface.DistScale * 0.13f, m_Ioface.DistScale * 0.24f);
			OptimizedParameter.ParamEyeROpen = SMOOTH_MEDIUM(OptimizedParameter.ParamEyeROpen, normalizedRightEAR);
		}

		/*// Eye smile based on MouthForm
		OptimizedParameter.ParamEyeForm = MathUtils::Normalize(OptimizedParameter.ParamMouthForm, -0.3, 0.9f);
		OptimizedParameter.ParamEyeLSmile = OptimizedParameter.ParamEyeForm; // both left | right are equal ^^
		OptimizedParameter.ParamEyeRSmile = OptimizedParameter.ParamEyeForm;*/

		// Eye smile based on AngleY
		float normalizedEyeForm = MathUtils::Normalize(OptimizedParameter.ParamAngleY, -20.0f, 10.0f);
		OptimizedParameter.ParamEyeForm = SMOOTH_MEDIUM(OptimizedParameter.ParamEyeForm, normalizedEyeForm);
		OptimizedParameter.ParamEyeLSmile = OptimizedParameter.ParamEyeForm; // both left | right are equal ^^
		OptimizedParameter.ParamEyeRSmile = OptimizedParameter.ParamEyeForm;

		// EyeBrowY, left & right value will be equal
		float optBrowLY = MathUtils::Normalize(m_Ioface.DistScale * m_Ioface.EyeBrowLY, 44.0f, 53.0f);
		float optBrowRY = MathUtils::Normalize(m_Ioface.DistScale * m_Ioface.EyeBrowRY, 44.0f, 53.0f);
		float avgBrow = (optBrowLY + optBrowRY) / 2.f;
		OptimizedParameter.ParamBrowLY = SMOOTH_SLOW(OptimizedParameter.ParamBrowLY, avgBrow);
		OptimizedParameter.ParamBrowRY = OptimizedParameter.ParamBrowLY; // same

		// EyeBrowForm follow EyeBrowY, but <= 0.0f
		OptimizedParameter.ParamBrowLForm = OptimizedParameter.ParamBrowLY < 0.0f ? OptimizedParameter.ParamBrowLY : 0.0f;
		OptimizedParameter.ParamBrowRForm = OptimizedParameter.ParamBrowLForm;

		// EyeBrowAngle follow EyeBrowForm
		OptimizedParameter.ParamBrowLAngle = OptimizedParameter.ParamBrowLForm;
		OptimizedParameter.ParamBrowRAngle = OptimizedParameter.ParamBrowLForm;

		// EyeBall X & Y
		float eyeBallX = 0.0f;
		float eyeBallY = 0.0f;
		if (MainGui::Get().Checkbox_EyeballFollowCursor.IsChecked())
		{
			// Update Eye Ball X & Y parameters based on cursor position on the screen
			int screenWidth, screenHeight;
			WindowsAPI::GetDesktopResolution(&screenWidth, &screenHeight);

			int mouseX, mouseY;
			if (WindowsAPI::GetMousePosition(&mouseX, &mouseY))
			{
				eyeBallX = MathUtils::Normalize(mouseX, screenWidth / 2, screenWidth) / 1.5f;
				eyeBallY = MathUtils::Normalize(mouseY, screenHeight / 2, screenHeight) / 1.5f;
			}
		}
		OptimizedParameter.ParamEyeBallX = eyeBallX;
		OptimizedParameter.ParamEyeBallY = -eyeBallY;
	}

	void Application::BindDefaultParametersWithFace()
	{
		ExampleAppLog::AddLog("[Iolive][I] Binding model parameters with Ioface\n\n");
		Model2D* model = m_UserModel.GetModel2D();
		const auto& paramIndex = model->GetParameterIndex();

		// check is parameter exist?, then bind it
		if (paramIndex.ParamAngleX > -1)
			model->SetParameterBindingAt(paramIndex.ParamAngleX, &(OptimizedParameter.ParamAngleX));
		if (paramIndex.ParamAngleY > -1)
			model->SetParameterBindingAt(paramIndex.ParamAngleY, &(OptimizedParameter.ParamAngleY));
		if (paramIndex.ParamAngleZ > -1)
			model->SetParameterBindingAt(paramIndex.ParamAngleZ, &(OptimizedParameter.ParamAngleZ));
		if (paramIndex.ParamBodyAngleX > -1)
			model->SetParameterBindingAt(paramIndex.ParamBodyAngleX, &(OptimizedParameter.ParamBodyAngleX));
		if (paramIndex.ParamBodyAngleY > -1)
			model->SetParameterBindingAt(paramIndex.ParamBodyAngleY, &(OptimizedParameter.ParamBodyAngleY));
		if (paramIndex.ParamBodyAngleZ > -1)
			model->SetParameterBindingAt(paramIndex.ParamBodyAngleZ, &(OptimizedParameter.ParamBodyAngleZ));
		if (paramIndex.ParamEyeLOpen > -1)
			model->SetParameterBindingAt(paramIndex.ParamEyeLOpen, &(OptimizedParameter.ParamEyeLOpen));
		if (paramIndex.ParamEyeROpen > -1)
			model->SetParameterBindingAt(paramIndex.ParamEyeROpen, &(OptimizedParameter.ParamEyeROpen));
		if (paramIndex.ParamEyeLSmile > -1)
			model->SetParameterBindingAt(paramIndex.ParamEyeLSmile, &(OptimizedParameter.ParamEyeLSmile));
		if (paramIndex.ParamEyeRSmile > -1)
			model->SetParameterBindingAt(paramIndex.ParamEyeRSmile, &(OptimizedParameter.ParamEyeRSmile));
		if (paramIndex.ParamEyeForm > -1)
			model->SetParameterBindingAt(paramIndex.ParamEyeForm, &(OptimizedParameter.ParamEyeForm));
		if (paramIndex.ParamEyeBallX > -1)
			model->SetParameterBindingAt(paramIndex.ParamEyeBallX, &(OptimizedParameter.ParamEyeBallX));
		if (paramIndex.ParamEyeBallY > -1)
			model->SetParameterBindingAt(paramIndex.ParamEyeBallY, &(OptimizedParameter.ParamEyeBallY));
		if (paramIndex.ParamMouthOpenY > -1)
			model->SetParameterBindingAt(paramIndex.ParamMouthOpenY, &(OptimizedParameter.ParamMouthOpenY));
		if (paramIndex.ParamMouthForm > -1)
			model->SetParameterBindingAt(paramIndex.ParamMouthForm, &(OptimizedParameter.ParamMouthForm));
		if (paramIndex.ParamBrowLY > -1)
			model->SetParameterBindingAt(paramIndex.ParamBrowLY, &(OptimizedParameter.ParamBrowLY));
		if (paramIndex.ParamBrowRY > -1)
			model->SetParameterBindingAt(paramIndex.ParamBrowRY, &(OptimizedParameter.ParamBrowRY));
		if (paramIndex.ParamBrowLForm > -1)
			model->SetParameterBindingAt(paramIndex.ParamBrowLForm, &(OptimizedParameter.ParamBrowLForm));
		if (paramIndex.ParamBrowRForm > -1)
			model->SetParameterBindingAt(paramIndex.ParamBrowRForm, &(OptimizedParameter.ParamBrowRForm));
		if (paramIndex.ParamBrowLAngle > -1)
			model->SetParameterBindingAt(paramIndex.ParamBrowLAngle, &(OptimizedParameter.ParamBrowLAngle));
		if (paramIndex.ParamBrowRAngle > -1)
			model->SetParameterBindingAt(paramIndex.ParamBrowRAngle, &(OptimizedParameter.ParamBrowRAngle));
	}

	void Application::BindDefaultParametersWithGui()
	{
		ExampleAppLog::AddLog("[Iolive][I] Binding model parameters with the GUI\n\n");

		Model2D* model = m_UserModel.GetModel2D();
		const auto& paramIndex = model->GetParameterIndex();

		ParameterScene paramGui = MainGui::Get().ParameterGUI;

		// check is parameter exist?, then bind it
		if (paramIndex.ParamAngleX > -1)
			model->SetParameterBindingAt(paramIndex.ParamAngleX, paramGui.GetPtrValueByIndex(paramIndex.ParamAngleX));
		if (paramIndex.ParamAngleY > -1)
			model->SetParameterBindingAt(paramIndex.ParamAngleY, paramGui.GetPtrValueByIndex(paramIndex.ParamAngleY));
		if (paramIndex.ParamAngleZ > -1)
			model->SetParameterBindingAt(paramIndex.ParamAngleZ, paramGui.GetPtrValueByIndex(paramIndex.ParamAngleZ));
		if (paramIndex.ParamBodyAngleX > -1)
			model->SetParameterBindingAt(paramIndex.ParamBodyAngleX, paramGui.GetPtrValueByIndex(paramIndex.ParamBodyAngleX));
		if (paramIndex.ParamBodyAngleY > -1)
			model->SetParameterBindingAt(paramIndex.ParamBodyAngleY, paramGui.GetPtrValueByIndex(paramIndex.ParamBodyAngleY));
		if (paramIndex.ParamBodyAngleZ > -1)
			model->SetParameterBindingAt(paramIndex.ParamBodyAngleZ, paramGui.GetPtrValueByIndex(paramIndex.ParamBodyAngleZ));
		if (paramIndex.ParamEyeLOpen > -1)
			model->SetParameterBindingAt(paramIndex.ParamEyeLOpen, paramGui.GetPtrValueByIndex(paramIndex.ParamEyeLOpen));
		if (paramIndex.ParamEyeROpen > -1)
			model->SetParameterBindingAt(paramIndex.ParamEyeROpen, paramGui.GetPtrValueByIndex(paramIndex.ParamEyeROpen));
		if (paramIndex.ParamEyeLSmile > -1)
			model->SetParameterBindingAt(paramIndex.ParamEyeLSmile, paramGui.GetPtrValueByIndex(paramIndex.ParamEyeLSmile));
		if (paramIndex.ParamEyeRSmile > -1)
			model->SetParameterBindingAt(paramIndex.ParamEyeRSmile, paramGui.GetPtrValueByIndex(paramIndex.ParamEyeRSmile));
		if (paramIndex.ParamEyeForm > -1)
			model->SetParameterBindingAt(paramIndex.ParamEyeForm, paramGui.GetPtrValueByIndex(paramIndex.ParamEyeForm));
		if (paramIndex.ParamEyeBallX > -1)
			model->SetParameterBindingAt(paramIndex.ParamEyeBallX, paramGui.GetPtrValueByIndex(paramIndex.ParamEyeBallX));
		if (paramIndex.ParamEyeBallY > -1)
			model->SetParameterBindingAt(paramIndex.ParamEyeBallY, paramGui.GetPtrValueByIndex(paramIndex.ParamEyeBallY));
		if (paramIndex.ParamMouthOpenY > -1)
			model->SetParameterBindingAt(paramIndex.ParamMouthOpenY, paramGui.GetPtrValueByIndex(paramIndex.ParamMouthOpenY));
		if (paramIndex.ParamMouthForm > -1)
			model->SetParameterBindingAt(paramIndex.ParamMouthForm, paramGui.GetPtrValueByIndex(paramIndex.ParamMouthForm));
		if (paramIndex.ParamBrowLY > -1)
			model->SetParameterBindingAt(paramIndex.ParamBrowLY, paramGui.GetPtrValueByIndex(paramIndex.ParamBrowLY));
		if (paramIndex.ParamBrowRY > -1)
			model->SetParameterBindingAt(paramIndex.ParamBrowRY, paramGui.GetPtrValueByIndex(paramIndex.ParamBrowRY));
		if (paramIndex.ParamBrowLForm > -1)
			model->SetParameterBindingAt(paramIndex.ParamBrowLForm, paramGui.GetPtrValueByIndex(paramIndex.ParamBrowLForm));
		if (paramIndex.ParamBrowRForm > -1)
			model->SetParameterBindingAt(paramIndex.ParamBrowRForm, paramGui.GetPtrValueByIndex(paramIndex.ParamBrowRForm));
		if (paramIndex.ParamBrowLAngle > -1)
			model->SetParameterBindingAt(paramIndex.ParamBrowLAngle, paramGui.GetPtrValueByIndex(paramIndex.ParamBrowLAngle));
		if (paramIndex.ParamBrowRAngle > -1)
			model->SetParameterBindingAt(paramIndex.ParamBrowRAngle, paramGui.GetPtrValueByIndex(paramIndex.ParamBrowRAngle));
	}
} // namespace Iolive