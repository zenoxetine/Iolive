#pragma once

#include <imgui.h>
#include "ParameterGui.hpp"
#include "Component/Checkbox.hpp"

// Access parent function
#include "../../Live2D/Live2DManager.hpp"
#include "../../Window.hpp"
#include "../Utility.hpp"

#include <string>
#include <map>

namespace Iolive {

class MainWidget
{
public:
	static void DoDraw()
	{
		ImGuiIO& io = ImGui::GetIO();
		ImGui::SetNextWindowSize(ImVec2(330, io.DisplaySize.y), ImGuiCond_Once);
		{
			ImGui::Begin("Main Widget");

			ImVec2 widgetSize = ImGui::GetWindowSize();

			if (ImGui::BeginTabBar("Main TabBar", ImGuiTabBarFlags_None))
			{
				if (ImGui::BeginTabItem("Model"))
				{
					// [Button] Open model
					ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 16.f);
					if (ImGui::Button(Live2DManager::IsModelInitialized() ? "Change Model" : "Open Model", ImVec2(widgetSize.x - 32, 32)))
						OpenNewModel();
					ImGui::PopStyleVar();

					// [ParameterGui]
					ParameterGui::DoDraw();

					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem("Face Capture"))
				{
					// [Checkbox] Face capture
					m_FaceCapture.DoDraw();

					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem("Settings"))
				{
					const char* fpsStr[FPS_COUNT] = { "30", "45", "60" };
					const char* fpsStrShowed = fpsStr[m_SelectedFPS];
					if (ImGui::SliderInt("Set FPS", &m_SelectedFPS, 0, FPS_COUNT-1, fpsStrShowed))
						Window::SetMaxFPS(FPS_VALUE[m_SelectedFPS]);

					ImGui::Text("Estimated FPS: %.0f", io.Framerate);

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
		std::wstring filePath = Utility::WOpenFileDialog(L"Live2D JSON File (*.model3.json)\000*.model3.json\000");
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
	static Checkbox& GetCheckbox_FaceCapture() { return m_FaceCapture; }
	static float GetSelectedFPS() { return FPS_VALUE[m_SelectedFPS]; }

public:
	inline static constexpr int FPS_COUNT = 3;
	inline static float FPS_VALUE[FPS_COUNT] = { 30.f, 45.f, 60.f };

private:
	inline static Checkbox m_FaceCapture = Checkbox("Enable Face Capture");
	inline static int m_SelectedFPS = FPS_COUNT-1;
};

} // namespace Iolive