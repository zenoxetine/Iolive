#pragma once

#include <thread>

namespace Iolive {
	class Application
	{
	public:
		static Application* Get()
		{
			static Application* staticApplication = nullptr;
			if (!staticApplication)
				staticApplication = new Application();

			return staticApplication;
		}

		static void Release()
		{
			delete Application::Get();
		}

		void Run();

	private:
		Application();
		~Application();

		void OnUpdate();
		void OnRender();

		//
		// do capturing new frame until camera got closed
		//
		void FaceCaptureLoop();
		
		bool OpenCamera();
		void CloseCamera();

		void BindDefaultParametersWithFace();
		void BindDefaultParametersWithGui();

	private:
		std::thread faceCaptureThread; // face capturing thread
	};
}