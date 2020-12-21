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
		// ImGui::SetNextWindowSize(ImVec2(300, io.DisplaySize.y * 1.01), ImGuiCond_Always);
		{
			ImGui::Begin("Ioleft Widget");

			// [Checkbox] Face capture
			m_FaceCapture.DoDraw();

			// [Checkbox] Show frame
			// m_ShowFrame.DoDraw();
			
			ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 14.f);
			// [Button] Open model
			if (ImGui::Button("Open Model", ImVec2(256, 32)))
				OpenNewModel();
			ImGui::PopStyleVar();

			// [ParamGui]
			m_ParamValue.DoDraw();

			ImGui::End();
		}
	}

	ParameterValue& GetParameterGui() { return m_ParamValue; }
	Checkbox& GetCheckboxFaceCapture() { return m_FaceCapture; }
	// Checkbox& GetCheckboxShowFrame() { return m_ShowFrame; }

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
	// private member
	ParameterValue m_ParamValue;
	Checkbox m_FaceCapture;
	// Checkbox m_ShowFrame;
};