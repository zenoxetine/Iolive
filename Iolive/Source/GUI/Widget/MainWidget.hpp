#pragma once

#include <imgui.h>
#include "ParameterGui.hpp"
#include "Component/Checkbox.hpp"

// Access parent function
#include "../../Window.hpp"
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include "../../Live2D/Live2DManager.hpp"
#include "../Utility.hpp"

#include <string>

namespace Iolive {

class MainWidget
{
public:
	MainWidget() = delete;

	static void DoDraw()
	{
		ImGuiIO& io = ImGui::GetIO();
		ImGui::SetNextWindowSize(ImVec2(320, io.DisplaySize.y), ImGuiCond_Once);
		{
			ImGui::Begin("Main Widget");

			ImVec2 widgetSize = ImGui::GetWindowSize();

			if (ImGui::BeginTabBar("Main TabBar", ImGuiTabBarFlags_None))
			{
				if (ImGui::BeginTabItem("Common"))
				{
					if (ImGui::CollapsingHeader("Face Capture", ImGuiTreeNodeFlags_DefaultOpen))
					{
						// [Checkbox] Face capture
						m_Checkbox_FaceCapture.DoDraw();

						if (m_Checkbox_FaceCapture.IsChecked())
						{
							m_Checkbox_EqualizeEyes.DoDraw();
						}
					}

					if (ImGui::CollapsingHeader("Model", ImGuiTreeNodeFlags_DefaultOpen))
					{
						// [Button] Open model
						ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 16.f);
						if (ImGui::Button(Live2DManager::IsModelInitialized() ? "Change Model" : "Open Model", ImVec2(widgetSize.x - 32, 32)))
							OpenNewModel();
						ImGui::PopStyleVar();

						// [ParameterGui]
						ParameterGui::DoDraw();
					}

					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem("Settings"))
				{
					const char* fpsStr[FPS_COUNT] = { "30", "45", "60", "75" };
					const char* fpsStrShowed = fpsStr[m_SelectedFPS];
					if (ImGui::SliderInt("Set FPS", &m_SelectedFPS, 0, FPS_COUNT-1, fpsStrShowed))
						Window::SetMaxFPS(FPS_VALUE[m_SelectedFPS]);

					ImGui::Text("Estimated FPS: %.0f", io.Framerate);

					ImGui::Text("Log:");
					ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
					ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 3.0f);
					if (ImGui::BeginChild("LogScene", ImVec2(ImGui::GetWindowWidth() - 15, 120), true))
					{
						LogScene.Draw();
						ImGui::EndChild();
					}
					ImGui::PopStyleVar();
					ImGui::PopStyleColor(2);

					ImGui::EndTabItem();
				}
				ImGui::EndTabBar();
			}

			ImGui::End();
		}
	}

private:
	/*
	* TODO: Action
	*/
	static void OpenNewModel()
	{
		std::wstring filePath = Utility::WOpenFileDialog(L"Live2D JSON File (*.model3.json)\000*.model3.json\000", glfwGetWin32Window(Window::GetWindow()));
		if (filePath.size() > 0)
		{
			// clear ParameterGui
			ParameterGui::ClearAll();

			// create new model!
			if (Live2DManager::SetModel(filePath.data()))
			{
				// success create model
				// give model parameter information to ParameterGui
				ParameterGui::SetParameterMap(Live2DManager::GetParameterMap(), Live2DManager::GetParameterMinMax());

				// set parameter binding between gui and live2d model
				for (size_t paramIndex = 0; paramIndex < ParameterGui::GetParameterCount(); paramIndex++)
				{
					Live2DManager::SetParameterBinding(paramIndex, ParameterGui::GetPtrValueByIndex(paramIndex));
				}
			}
		}
	}

public:
	static Checkbox& GetCheckbox_FaceCapture() { return m_Checkbox_FaceCapture; }
	static Checkbox& GetCheckbox_EqualizeEyes() { return m_Checkbox_EqualizeEyes; }
	static float GetSelectedFPS() { return FPS_VALUE[m_SelectedFPS]; }

public:
	inline static constexpr int FPS_COUNT = 4;
	inline static float FPS_VALUE[FPS_COUNT] = { 30.f, 45.f, 60.f, 75.f };

private:
	inline static Checkbox m_Checkbox_FaceCapture = Checkbox("Enable Face Capture", false);
	inline static Checkbox m_Checkbox_EqualizeEyes = Checkbox("Equalize eye parameters", true);
	inline static int m_SelectedFPS = 2; // 60 FPS

public:
	// Log scene
	inline static struct ExampleAppLog {
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
			Buf.append(text);
		}

		static void Draw()
		{
			ImGui::TextUnformatted(Buf.begin());
			if (ScrollToBottom)
				ImGui::SetScrollHere(1.0f);
			ScrollToBottom = false;
		}
	} LogScene;
};

} // namespace Iolive