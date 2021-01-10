#pragma once

#include <imgui.h>
#include <imgui_internal.h>

#include <array>
#include <vector>
#include <stdio.h>
#include <algorithm>

// refrence: https://www.glfw.org/docs/latest/group__keys.html
constexpr char* const GlfwKeyNames[] = {
	"ESCAPE", //256
	"ENTER", //257
	"TAB", //258
	"BACKSPACE", //259
	"INSERT", //260
	"DELETE", //261
	"RIGHT", //262
	"LEFT", //263
	"DOWN", //264
	"UP", //265
	"PAGE_UP", //266
	"PAGE_DOWN", //267
	"HOME", //268
	"END", //269
	"UNKNOWN", //270
	"UNKNOWN", //271
	"UNKNOWN", //272
	"UNKNOWN", //273
	"UNKNOWN", //274
	"UNKNOWN", //275
	"UNKNOWN", //276
	"UNKNOWN", //277
	"UNKNOWN", //278
	"UNKNOWN", //279
	"CAPS_LOCK", //280
	"SCROLL_LOCK", //281
	"NUM_LOCK", //282
	"PRINT_SCREEN", //283
	"PAUSE", //284
	"UNKNOWN", //285
	"UNKNOWN", //286
	"UNKNOWN", //287
	"UNKNOWN", //288
	"UNKNOWN", //289
	"F1", //290
	"F2", //291
	"F3", //292
	"F4", //293
	"F5", //294
	"F6", //295
	"F7", //296
	"F8", //297
	"F9", //298
	"F10", //299
	"F11", //300
	"F12", //301
	"F13", //302
	"F14", //303
	"F15", //304
	"F16", //305
	"F17", //306
	"F18", //307
	"F19", //308
	"F20", //309
	"F21", //310
	"F22", //311
	"F23", //312
	"F24", //313
	"F25", //314
	"UNKNOWN", //315
	"UNKNOWN", //316
	"UNKNOWN", //317
	"UNKNOWN", //318
	"UNKNOWN", //319
	"KP_0", //320
	"KP_1", //321
	"KP_2", //322
	"KP_3", //323
	"KP_4", //324
	"KP_5", //325
	"KP_6", //326
	"KP_7", //327
	"KP_8", //328
	"KP_9", //329
	"KP_DECIMAL", //330
	"KP_DIVIDE", //331
	"KP_MULTIPLY", //332
	"KP_SUBTRACT", //333
	"KP_ADD", //334
	"KP_ENTER", //335
	"KP_EQUAL", //336
	"UNKNOWN", //337
	"UNKNOWN", //338
	"UNKNOWN", //339
	"LEFT_SHIFT", //340
	"LEFT_CONTROL", //341
	"LEFT_ALT", //342
	"LEFT_SUPER", //343
	"RIGHT_SHIFT", //344
	"RIGHT_CONTROL", //345
	"RIGHT_ALT", //346
	"RIGHT_SUPER", //347
	"MENU" //348
};

static constexpr int MAX_KEYS = 3;
typedef std::array<unsigned short, MAX_KEYS> HotkeyItem;

template<typename T>
class Hotkeys
{
public:
	Hotkeys() = default;

	void DrawGuiEdit(int editIndex, void(*callback)(int index, T item))
	{
		if (editIndex >= m_Keys.size()) return;

		ImGuiIO& io = ImGui::GetIO();

		static HotkeyItem tmpHotkeyItem;
		static bool tmpCopied = false;

		if (!tmpCopied)
		{
			tmpHotkeyItem = m_Keys[editIndex]; // copy to tmp
			tmpCopied = true;
		}

		auto pressedKeys = CheckKeyPressed();
		for (auto& key : pressedKeys)
		{
			bool doContinue = false;
			for (auto& tmpHotKey : tmpHotkeyItem)
				if (key == tmpHotKey)
					doContinue = true;
			if (doContinue) continue;

			for (auto& tmpHotKey : tmpHotkeyItem)
			{
				if (tmpHotKey == '\0')
				{
					tmpHotKey = key;
					break;
				}
			}
		}

		ImGui::SetNextWindowSize(ImVec2(420, 140), ImGuiCond_Once);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(420, 140));
		{
			// [Window] Edit Hotkeys 
			ImGui::Begin("Edit Hotkeys");

			ImGui::Text("* Press any key");
			ImGui::SameLine();
			ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10.f);
			if (ImGui::Button("Clear", ImVec2(52, 24)))
			{
				for (auto& key : tmpHotkeyItem)
					key = '\0';
			}
			ImGui::PopStyleVar();

			DrawHotkeyItem(tmpHotkeyItem);

			ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 13.f);
			if (ImGui::Button("Cancel", ImVec2(100, 30)))
			{
				tmpCopied = false;

				// do callback
				if (callback != nullptr)
				{
					callback(-1, nullptr);
				}

			} ImGui::SameLine();
			if (ImGui::Button("Save", ImVec2(100, 30)))
			{
				// Check if Hotkeys was used? ...

				// pass tmp hotkey
				m_Keys[editIndex] = tmpHotkeyItem;

				tmpCopied = false;

				// do callback
				if (callback != nullptr)
				{
					callback(editIndex, m_TypeItems[editIndex]);
				}
			}
			ImGui::PopStyleVar();

			ImGui::End();
		} // End ImGui window
		ImGui::PopStyleVar(); // Window min size
	}

	T Update()
	{
		if (Size() == 0) return nullptr;

		static std::vector<unsigned short> lastPressedKeys;
		static bool hotKeysHasActive = false;

		auto currentPressedKeys = CheckKeyPressed();
		if (currentPressedKeys.size() == 0)
		{
			hotKeysHasActive = false;
			return nullptr;
		}

		for (auto& currentKey : currentPressedKeys)
		{
			bool found = false;
			for (auto& lastKey : lastPressedKeys)
				if (currentKey == lastKey)
					found = true;

			if (!found)
			{
				lastPressedKeys.push_back(currentKey);
			}
		}

		if (currentPressedKeys.size() == lastPressedKeys.size())
		{
			for (int i = 0; i < lastPressedKeys.size(); i++)
			{
				if (i == MAX_KEYS) break;

				/*
				* Here is, keys can be pressed at the same time
				*/

				for (int hotKeyIndex = 0; hotKeyIndex < m_Keys.size(); hotKeyIndex++)
				{
					// get hotkeys size (size without '\0')
					int hotKeySize = 0;
					for (auto& key : m_Keys[hotKeyIndex])
						if (key != '\0')
							hotKeySize++;

					// is pressedKeys equal with hotkeys
					bool isKeysEqual = false;
					if (hotKeySize == lastPressedKeys.size())
					{
						isKeysEqual = true; // waitt!

						bool found = false;
						for (auto& key : m_Keys[hotKeyIndex])
						{
							if (key == '\0') break;
							if (std::find(lastPressedKeys.begin(), lastPressedKeys.end(), key) != lastPressedKeys.end())
							{
								found = true;
							}
							else
							{
								found = false;
								break;
							}
						}

						if (!found)
							isKeysEqual = false; // nah
					}

					if (isKeysEqual)
					{
						// Here is the Hotkeys x got pressed

						if (!hotKeysHasActive)
						{
							hotKeysHasActive = true;
							return m_TypeItems[hotKeyIndex];
						}
					}
				}
			}
		}
		else
		{
			lastPressedKeys.clear();
			hotKeysHasActive = false;
		}
		return nullptr;
	}

	void DrawHotkeyItem(HotkeyItem& hotkeyItem, float widthFactor = 0.88f)
	{
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.95f, 0.95f, 0.95f, 1.0f));
		float keyButton_Width = (ImGui::GetWindowWidth() / (float)MAX_KEYS) * widthFactor;
		for (int i = 0; i < MAX_KEYS; i++)
		{
			if (hotkeyItem[i] != '\0')
			{
				if (hotkeyItem[i] > 255 && hotkeyItem[i] < 349)
				{
					// showing non-alphanumeric characters, defined in GlfwKeyNames
					const char* keyName = GlfwKeyNames[hotkeyItem[i] - 256];
					if (keyName)
					{
						ImGui::Button(keyName, ImVec2(keyButton_Width, 32));
					}
				}
				else
				{
					// showing alpha-numeric characters
					char keyCharName[2]; // +1 for null terminated char
					std::sprintf(keyCharName, "%c\0", hotkeyItem[i]);
					ImGui::Button(keyCharName, ImVec2(keyButton_Width, 32));
				}
			}
			else
			{
				ImGui::Button("...", ImVec2(keyButton_Width, 32));
			}

			if (i < (MAX_KEYS - 1)) ImGui::SameLine();
		}
		ImGui::PopItemFlag();
		ImGui::PopStyleColor();
	}

	void AddHotkeysAndItem(const HotkeyItem& hotkeyItem, T typeItem)
	{
		m_Keys.push_back(hotkeyItem);
		m_TypeItems.push_back(typeItem);
	}

	int Size() const
	{
		return m_Keys.size();
	}

	HotkeyItem& GetHotkeysAtIndex(int index)
	{
		return m_Keys[index];
	}

	T GetItemAtIndex(int index)
	{
		return m_TypeItems[index];
	}

	void ClearAll()
	{
		m_Keys.clear();
		m_TypeItems.clear();
	}

private:
	std::vector<unsigned short> CheckKeyPressed()
	{
		ImGuiIO& io = ImGui::GetIO();

		std::vector<unsigned short> pressedKeys;

		for (int i = 0; i < 512; i++)
		{
			if (io.KeysDown[i] == true)
			{
				pressedKeys.push_back(i);
			}
		}

		return pressedKeys;
	}
private:
	std::vector<HotkeyItem> m_Keys;
	std::vector<T> m_TypeItems;
};