#pragma once

#include <imgui.h>
#include <map>
#include <vector>
#include <array>

class ParameterValue
{
public:
	ParameterValue()
	{
	}

	void DoDraw()
	{
		if (GetParameterCount() < 1) return;

		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.f);
		ImGui::PushItemWidth(150);
		size_t paramIndex = 0;
		for (auto&[key, value] : m_Parameters)
		{
			ImGui::SliderFloat(key, &value,
				m_ParamMinMax[paramIndex][0], m_ParamMinMax[paramIndex][1],
				"%.2f"
			);
			paramIndex++;
		}
		ImGui::PopItemWidth();
		ImGui::PopStyleVar();
	}

	void SetParameterMap(const std::map<const char*, float>& paramMap, const std::vector<std::array<float, 2>>& paramMinMax)
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

	float* GetPtrValueByIndex(int index)
	{
		return m_ParametersPtrValue[index];
	}

	void ClearAll()
	{
		m_ParametersPtrValue.clear();
		m_Parameters.clear();
		m_ParamMinMax.clear();
	}

	size_t GetParameterCount() const
	{
		return m_Parameters.size();
	}

private:
	std::map<const char*, float> m_Parameters;
	std::vector<float*> m_ParametersPtrValue; // this is pointed to m_Parameters second value

	std::vector<std::array<float, 2>> m_ParamMinMax;
};