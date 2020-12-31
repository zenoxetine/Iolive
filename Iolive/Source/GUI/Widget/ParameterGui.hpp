#pragma once

#include <imgui.h>
#include <imgui_internal.h>
#include <map>
#include <vector>
#include <array>

// Access parent function
#include "../../Live2D/Live2DManager.hpp"

namespace Iolive {

class ParameterGui
{
public:
	ParameterGui() = delete;

	static void DoDraw()
	{
		if (GetParameterCount() < 1) return;

		ImGui::PushItemWidth(150);

		std::map<int, float*>& modelBindedParameter = Live2DManager::GetBindedParameterMap();

		size_t paramIndex = 0;
		for (auto&[name, value] : m_Parameters)
		{
			bool isBindedWithGUI = &value == modelBindedParameter[paramIndex];
			if (!isBindedWithGUI)
			{
				// this parameter is not binded with the GUI. Disable this parameter
				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.55f);
			}

			ImGui::SliderFloat(name, modelBindedParameter[paramIndex],
				m_ParamMinMax[paramIndex][0], m_ParamMinMax[paramIndex][1],
				"%.2f"
			);
			
			if (!isBindedWithGUI)
			{
				ImGui::PopItemFlag();
				ImGui::PopStyleVar();
			}

			paramIndex++;
		}
		ImGui::PopItemWidth();
	}

	static void SetParameterMap(const std::map<const char*, float>& paramMap, const std::vector<std::array<float, 2>>& paramMinMax)
	{
		ClearAll();

		// assign and then don't use paramMap
		m_Parameters = paramMap;

		// vector of pointer to m_Parameters second value
		m_ParametersPtrValue.reserve(GetParameterCount());
		for (auto& [_key, value] : m_Parameters)
			m_ParametersPtrValue.push_back(&value);

		m_ParamMinMax = paramMinMax;
	}

	static float* GetPtrValueByIndex(int index)
	{
		return m_ParametersPtrValue[index];
	}

	static void ClearAll()
	{
		m_ParametersPtrValue.clear();
		m_Parameters.clear();
		m_ParamMinMax.clear();
	}

	static size_t GetParameterCount()
	{
		return m_Parameters.size();
	}

private:
	inline static std::map<const char*, float> m_Parameters;
	inline static std::vector<float*> m_ParametersPtrValue; // this is pointed to m_Parameters second value

	inline static std::vector<std::array<float, 2>> m_ParamMinMax;
};

} // namespace Iolive