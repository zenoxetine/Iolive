#pragma once

#include "Ioface/Ioface.hpp"

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
		
		bool OpenCamera();
		void CloseCamera();

		void BindDefaultParametersWithFace();
		void BindDefaultParametersWithGui();

	private:
		Ioface m_Ioface;
	};
}