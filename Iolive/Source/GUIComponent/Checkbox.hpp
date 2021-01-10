#pragma once

#include <imgui.h>
#include <mutex>

struct Checkbox
{
public:
	inline static std::mutex s_GetterGuard;
	Checkbox(const char* name, bool checked = false)
	  :	m_Name(name), m_Checked(checked) {}
	
	/*
	* Draw checkbox
	* \return true when checkbox got clicked
	*/
	bool Draw()
	{
		if (ImGui::Checkbox(m_Name, &m_Checked))
			return true;
		else
			return false;
	}

	// getter & setter
	bool IsChecked() const {
		std::lock_guard<std::mutex> lock(s_GetterGuard);
		return m_Checked;
	}
	void SetChecked(bool checked) { m_Checked = checked; }

	bool* GetPtrChecked() { return &m_Checked; }

private:
	const char* m_Name;
	bool m_Checked;
};