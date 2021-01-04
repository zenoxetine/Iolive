#pragma once

#define _HAS_STD_BYTE 0 // this can fix "byte is ambigous between windows.h & c++17"
#define IMGUI_IMPL_OPENGL_LOADER_GLEW
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "Application.hpp"
#include "Live2D/Model2D.hpp"
#include "Utility/DeviceEnumerator.h"

// Components
#include "GUIComponent/Checkbox.hpp"

#include <map>
#include <vector>
#include <array>
#include <mutex>

namespace Iolive {
	class Application;

	/*
	* Log scene *static
	*/
	struct ExampleAppLog {
	private:
		inline static ImGuiTextBuffer Buf;
		inline static bool ScrollToBottom;
	public:
		static void Clear() { Buf.clear(); }

		static void AddLogf(const char* fmt, ...) {
			va_list args;
			va_start(args, fmt);
			Buf.appendfv(fmt, args);
			va_end(args);
			ScrollToBottom = true;
		}

		static void AddLog(const char* text) {
			if (Buf.size() > 32000)
			{
				Buf.clear();
				Buf.append("Clearing buffer.\n\n");
			}

			Buf.append(text);
		}

		static void Draw()
		{
			ImGui::TextUnformatted(Buf.begin());
			if (ScrollToBottom)
				ImGui::SetScrollHere(1.0f);
			ScrollToBottom = false;
		}
	};
	
	/*
	* Model Parameter Scene
	*/
	class ParameterScene
	{
	public:
		ParameterScene();

		void SetModel(Model2D* model);
		void UnsetModel();

		void Draw();

		float* GetPtrValueByIndex(int index);
		int GetParameterSize() const;

	private:
		void ClearParameters();

	private:
		Model2D* m_Model2D;

		std::map<const char*, float> m_Parameters;
		std::vector<float*> m_ParametersPtrValue; // this is pointed to m_Parameters second value

		std::vector<std::array<float, 2>> m_ParamMinMax;
	};

	/*
	* MainGui Skeleton
	* - Initialize ImGui first, then get the instance by calling Get() function
	*/
	class MainGui
	{
	public:
		static void InitializeImGui(GLFWwindow* window);
		static void ShutdownImGui();

		inline static std::mutex mtxGetGuiInstance;
		static MainGui& Get();
	private:
		static void SetupImGuiStlye();

	public:
		void Draw(Application* app);
	private:
		MainGui() = default;
		void BeginImGuiFrame();
		void EndImGuiFrame();

	public:
		/* * *
		* GUI Data
		* named without m_
		*/

		Checkbox Checkbox_FaceCapture = Checkbox("Enable Face Capture", false);
		Checkbox Checkbox_ShowFrame = Checkbox("Show Frame", false);
		Checkbox Checkbox_EqualizeEyes = Checkbox("Equalize Eye Parameters", true);
		Checkbox Checkbox_EyeballFollowCursor = Checkbox("Eye Ball Follow Cursor", true);
		Checkbox Checkbox_WindowVisible = Checkbox("Window Visible", true);

		ParameterScene ParameterGUI;

		DeviceEnumerator DeviceEnumerator; // DeviceEnumerator instance
		std::map<int, Device> CameraDevicesMap; // Camera Devices map
		int SelectedCameraId = 0; // default camera is 0

		float ColorEdit_ClearColor[3] = { 0.22f, 1.0f, 0.07f }; // default neon green.
	};
}