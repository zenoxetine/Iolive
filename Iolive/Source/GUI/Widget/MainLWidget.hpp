#pragma once

#include <imgui.h>
#include "Component/ParameterValue.hpp"
#include "Component/Checkbox.hpp"
#include "../../Live2D/Live2DManager.hpp"
#include "../Utility.hpp"
#include <string>
#include <map>

class MainLeftWidget
{
public:
	MainLeftWidget()
	  :	m_ParamValue(ParameterValue()),
		m_FaceCapture(Checkbox("Face Capture"))
		// m_ShowFrame(Checkbox("Show Frame"))
	{
	}

	void DoDraw()
	{
		ImGuiIO& io = ImGui::GetIO();
		ImGui::SetNextWindowSize(ImVec2(300, io.DisplaySize.y), ImGuiCond_Once);
		{
			ImGui::Begin("Ioleft Widget");

			if (ImGui::BeginTabBar("Ioleft TabBar", ImGuiTabBarFlags_None))
			{
				if (ImGui::BeginTabItem("Model"))
				{
					// [Button] Open model
					ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 14.f);
					if (ImGui::Button("Open Model", ImVec2(256, 32)))
						OpenNewModel();
					ImGui::PopStyleVar();

					// [ParamGui]
					m_ParamValue.DoDraw();
					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem("Face Capture"))
				{
					// [Checkbox] Face capture
					m_FaceCapture.DoDraw();

					// [Checkbox] Show frame
					// m_ShowFrame.DoDraw();
					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem("Settings"))
				{
					const char* fpsStr[FPS_ITEM_COUNT] = { "30", "45", "60" };
					const char* fpsStrShowed = (m_SelectedFPS >= 0 && m_SelectedFPS < FPS_ITEM_COUNT) ? fpsStr[m_SelectedFPS] : "60";
					ImGui::SliderInt("Max FPS", &m_SelectedFPS, 0, FPS_ITEM_COUNT - 1, fpsStrShowed);
					ImGui::Text("Estimated FPS: %.0f", io.Framerate);

					ImGui::EndTabItem();
				}
				ImGui::EndTabBar();
			}

			ImGui::End();
		}
	}

	ParameterValue& GetParameterGui() { return m_ParamValue; }
	Checkbox& GetCheckboxFaceCapture() { return m_FaceCapture; }
	// Checkbox& GetCheckboxShowFrame() { return m_ShowFrame; }
	unsigned short GetSelectedFPS() { return m_SelectedFPS; }

public:
	enum FPSEnumValue { FPS_30 = 0, FPS_45, FPS_60, FPS_ITEM_COUNT };

private:
	// do action
	void OpenNewModel()
	{
		std::wstring filePath = Utility::WOpenFileDialog(L"Live2D JSON File (*.model3.json)\000*.model3.json\000");
		if (filePath.size() > 0)
		{
			// file selected
			// create new model
			if (Live2DManager::SetModel(filePath.data()))
			{
				// success create model
				// give model parameter information to ParamValue gui
				m_ParamValue.SetParameterMap(Live2DManager::GetParameterMap(), Live2DManager::GetParameterMinMax());

				// set parameter binding between gui and live2d model
				for (size_t paramIndex = 0; paramIndex < m_ParamValue.GetParameterCount(); paramIndex++)
				{
					Live2DManager::SetParameterBinding(paramIndex, m_ParamValue.GetPtrValueByIndex(paramIndex));
				}
			}
			else
			{
				m_ParamValue.ClearAll();
			}
		}
	}
	
private:
	ParameterValue m_ParamValue;
	Checkbox m_FaceCapture;
	// Checkbox m_ShowFrame;
	int m_SelectedFPS = FPS_60;
};