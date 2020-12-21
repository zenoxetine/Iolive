#pragma once

#include <imgui.h>

class Checkbox
{
public:
	Checkbox(const char* name, bool checked = false)
	  :	m_Name(name), m_Checked(checked), m_Changed(false)
	{
	}
	
	void DoDraw()
	{
		m_Changed = false;
		if (ImGui::Checkbox(m_Name, &m_Checked))
			m_Changed = true;
	}

public:
	// getter & setter
	bool IsChanged() const { return m_Changed; }
	bool IsChecked() const { return m_Checked; }
	void SetChecked(bool checked) { m_Checked = checked; }

private:
	const char* m_Name;
	bool m_Changed;
	bool m_Checked;
};