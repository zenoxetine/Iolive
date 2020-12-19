#pragma once

#include <imgui.h>
#include "Component/ParameterValue.hpp"
#include "../../Live2D/Live2DManager.hpp"
#include "../../Application.hpp"
#include "../Utility.hpp"
#include <string>
#include <map>

class MainLeftWidget
{
public:
	MainLeftWidget()
	  :	m_ParamValue(ParameterValue()),
		m_CameraChecked(false),
		m_CameraCheckboxChanged(false)
	{
	}

	void DoDraw()
	{
		{
			ImGui::Begin("Left Widget");

			// [Button] Open model
			if (ImGui::Button("Open Model"))
				OpenNewModel();

			// [Checkbox] Open camera
			m_CameraCheckboxChanged = false;
			if (ImGui::Checkbox("Open Camera", &m_CameraChecked))
				m_CameraCheckboxChanged = true;

			// [Parameter Gui]
			m_ParamValue.DoDraw();

			ImGui::End();
		}
	}

	ParameterValue& GetParameterGui() { return m_ParamValue; }

	bool CameraCheckboxChanged() const { return m_CameraCheckboxChanged; }
	bool IsCameraChecked() const { return m_CameraChecked; }
	void SetCameraChecked(bool checked) { m_CameraChecked = checked; }

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
	bool m_CameraChecked;

	// flags
	bool m_CameraCheckboxChanged;
};