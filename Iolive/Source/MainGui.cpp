#include "MainGui.hpp"
#include "Utility/WindowsAPI.hpp"
#include "Live2D/Live2DManager.hpp"
#include <string>

namespace Iolive {
	void MainGui::InitializeImGui(GLFWwindow* window)
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGuiIO& io = ImGui::GetIO(); (void)io;
		// io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;   // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport / Platform Windows
		io.ConfigViewportsNoAutoMerge = true; // fly... me to the moon~
		io.IniFilename = NULL; // don't create file .ini

		io.Fonts->AddFontFromFileTTF("Assets/fonts/roboto-android/Roboto-Bold.ttf", 18.0f);
		io.FontDefault = io.Fonts->AddFontFromFileTTF("Assets/fonts/roboto-android/Roboto-Regular.ttf", 18.0f);

		ImGui::StyleColorsLight();

		SetupImGuiStlye();

		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 330");
	}

	void MainGui::SetupImGuiStlye()
	{
		// make a good imgui style here
		ImGuiStyle& style = ImGui::GetStyle();

		// Check marks
		style.Colors[ImGuiCol_CheckMark] = ImVec4(0.05f, 0.80f, 0.05f, 1.0f);

		// Text
		style.Colors[ImGuiCol_Text] = ImVec4(color_for_text.x, color_for_text.y, color_for_text.z, 1.0f);
		style.Colors[ImGuiCol_TextDisabled] = ImVec4(color_for_text.x, color_for_text.y, color_for_text.z, 0.6f);
		style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 0.50f);

		// Window body
		style.Colors[ImGuiCol_WindowBg] = ImVec4(color_for_body.x, color_for_body.y, color_for_body.z, 1.0f);
		style.Colors[ImGuiCol_Border] = ImVec4(color_for_body.x, color_for_body.y, color_for_body.z, 0.00f);
		style.Colors[ImGuiCol_BorderShadow] = ImVec4(color_for_body.x, color_for_body.y, color_for_body.z, 0.00f);

		// Frame background
		style.Colors[ImGuiCol_FrameBg] = ImVec4(color_for_area.x, color_for_area.y, color_for_area.z, 1.0f);
		style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 0.7f);
		style.Colors[ImGuiCol_FrameBgActive] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 0.86f);

		// Window header
		style.Colors[ImGuiCol_TitleBgActive] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 1.00f);

		// ImGui component header
		style.Colors[ImGuiCol_Header] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 0.76f);
		style.Colors[ImGuiCol_HeaderHovered] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 0.86f);
		style.Colors[ImGuiCol_HeaderActive] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 1.00f);

		// Scroll bar
		style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(color_for_area.x, color_for_area.y, color_for_area.z, 1.00f);
		style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 0.5f);
		style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 0.75f);
		style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 1.00f);

		// Button
		style.Colors[ImGuiCol_Button] = ImVec4(color_for_button.x, color_for_button.y, color_for_button.z, 0.70f);
		style.Colors[ImGuiCol_ButtonHovered] = ImVec4(color_for_button.x, color_for_button.y, color_for_button.z, 0.86f);
		style.Colors[ImGuiCol_ButtonActive] = ImVec4(color_for_button.x, color_for_button.y, color_for_button.z, 1.0f);

		// Slider
		style.Colors[ImGuiCol_SliderGrab] = ImVec4(color_for_slider.x, color_for_slider.y, color_for_slider.z, 0.75f);
		style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(color_for_slider.x, color_for_slider.y, color_for_slider.z, 1.0f);

		// Popup
		style.Colors[ImGuiCol_PopupBg] = ImVec4(color_for_area.x, color_for_area.y, color_for_area.z, 1.0f);

		// Resize grip
		style.Colors[ImGuiCol_ResizeGrip] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 0.90f);
		style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 0.75f);
		style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 1.00f);

		// Tab
		style.Colors[ImGuiCol_Tab] = ImVec4(color_for_tab.x, color_for_tab.y, color_for_tab.z, 1.0f);
		style.Colors[ImGuiCol_TabHovered] = ImVec4(color_for_tab.x, color_for_tab.y, color_for_tab.z, 0.65f);
		style.Colors[ImGuiCol_TabActive] = ImVec4(color_for_tab.x, color_for_tab.y, color_for_tab.z, 0.65f);
	}

	void MainGui::ShutdownImGui()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	MainGui& MainGui::Get()
	{
		std::lock_guard<std::mutex> lock(s_MtxGetGuiInstance);
		static MainGui s_MainGui;
		return s_MainGui;
	}

	void MainGui::BeginImGuiFrame()
	{
		// starts new imgui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void MainGui::EndImGuiFrame()
	{
		// rendering
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
	}

	void MainGui::Draw(Application* app)
	{
		BeginImGuiFrame();

		int width, height;
		app->m_Window->GetWindowSize(&width, &height);
		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2(width, height);

		ImGui::SetNextWindowSize(ImVec2(320, 500), ImGuiCond_Once);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(250, 400));
		{
			ImGui::Begin("Main Iolive GUI");
			ImVec2 widgetSize = ImGui::GetWindowSize();

			if (ImGui::BeginTabBar("##MainTabBar", ImGuiTabBarFlags_None))
			{
				/*
				* Model tab
				*/
				if (ImGui::BeginTabItem("Model"))
				{
					// [Button] Open model
					ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 16.f);
					if (ImGui::Button(app->m_UserModel.IsModelInitialized() ? "Change Model" : "Open Model", ImVec2(widgetSize.x - 30, 32)))
					{
						// Open new model
						std::wstring filePath = WindowsAPI::WOpenFileDialog(
							L"Live2D JSON File (*.model3.json)\000*.model3.json\000",
							glfwGetWin32Window(app->m_Window->GetGlfwWindow())
						);

						if (filePath.size() > 0) // file selected
						{
							// delete previous model
							app->m_UserModel.DeleteModel();
							
							// clear ParameterGUI
							ParameterGUI.UnsetModel();

							// clear hotkeys
							MainGui::Get().GuiHotkeys.ClearAll();

							Model2D* newModel = Live2DManager::CreateModel(filePath.data());
							if (newModel)
							{
								// success create model
								app->SetModel(newModel);
							}
						}
					}
					ImGui::PopStyleVar(); // End button style var

					if (app->m_UserModel.IsModelInitialized())
					{
						Checkbox_ShowModelHotkeys.Draw();

						if (ImGui::CollapsingHeader("Parameters"))
						{
							// Parameter Scene
							ParameterGUI.Draw();
						}
					}

					ImGui::EndTabItem();
				}

				/*
				* Face capture tab
				*/
				if (ImGui::BeginTabItem("Face Capture"))
				{
					/*
					* List of camera devices
					*/
					static bool firstTime = true;
					if (firstTime)
					{
						// first time, get devices map
						CameraDevicesMap = DeviceEnumerator.getVideoDevicesMap();
						firstTime = false;
					}

					if (Checkbox_FaceCapture.IsChecked())
					{
						// Disable when face capture is active
						ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
						ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.55f);
					}
					if (ImGui::BeginCombo("##CameraDevices", CameraDevicesMap[SelectedCameraId].deviceName.c_str()))
					{
						CameraDevicesMap = DeviceEnumerator.getVideoDevicesMap(); // update device map
						for (int i = 0; i < CameraDevicesMap.size(); i++)
						{
							bool isSelected = (SelectedCameraId == CameraDevicesMap[i].id);
							if (ImGui::Selectable(CameraDevicesMap[i].deviceName.c_str(), isSelected))
							{
								// selected by user
								SelectedCameraId = CameraDevicesMap[i].id;
								if (isSelected)
									ImGui::SetItemDefaultFocus();
							}
						}
						ImGui::EndCombo();
					}
					if (Checkbox_FaceCapture.IsChecked())
					{
						ImGui::PopItemFlag();
						ImGui::PopStyleVar();
					}
					// End list of camera devices

					// Checkbox enable face capture
					if (Checkbox_FaceCapture.Draw())
					{
						if (Checkbox_FaceCapture.IsChecked())
						{
							if (!app->OpenCamera())
								Checkbox_FaceCapture.SetChecked(false);
						}
						else
						{
							app->CloseCamera();

							// next time, don't automatically showing the frame!
							Checkbox_ShowFrame.SetChecked(false);
							Checkbox_ShowFace.SetChecked(false);
						}
					}

					if (Checkbox_FaceCapture.IsChecked())
					{
						ImGui::Spacing(); ImGui::SameLine();
						ImGui::PushItemWidth(ImGui::GetWindowSize().x / 2.25);
						ImGui::SliderInt("Tracking delay", &(app->m_Ioface.TrackingDelay), 0, 40,
							app->m_Ioface.TrackingDelay == 0 ? "No delay" : "%dms"
						);
						ImGui::PopItemWidth();

						ImGui::Spacing(); ImGui::SameLine();
						if (Checkbox_ShowFrame.Draw())
						{
							if (!Checkbox_ShowFrame.IsChecked())
							{
								Checkbox_ShowFace.SetChecked(false);
							}
						}

						if (Checkbox_ShowFrame.IsChecked())
						{
							ImGui::Spacing(); ImGui::SameLine();
							ImGui::Spacing(); ImGui::SameLine();
							Checkbox_ShowFace.Draw();
						}

						ImGui::Spacing(); ImGui::SameLine();
						Checkbox_EqualizeEyes.Draw(); // Checkbox equalize eye parameter

						ImGui::Spacing(); ImGui::SameLine();
						Checkbox_EyeballFollowCursor.Draw(); // Checkbox eye ball follow cursor
					}

					ImGui::EndTabItem();
				}

				/*
				* Settings tab
				*/
				if (ImGui::BeginTabItem("Settings"))
				{
					ImGui::PushItemWidth(ImGui::GetWindowSize().x / 2.25);
					ImGui::ColorEdit3("Clear Color", ColorEdit_ClearColor, ImGuiColorEditFlags_DisplayRGB);

					if (Checkbox_WindowVisible.Draw())
					{
						if (Checkbox_WindowVisible.IsChecked())
							app->m_Window->SetWindowOpacity(1.0f);
						else
							app->m_Window->SetWindowOpacity(0.0f);
					}

					ImGui::SliderFloat("Max FPS (not accurate)", &(app->m_Window->MaxFPS), 20.0f, 90.0f, "%.0f");
					ImGui::PopItemWidth();

					ImGui::Text("Estimated FPS: %.0f", io.Framerate);

					ImGui::Text("Log:");
					ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
					ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 3.0f);
					if (ImGui::BeginChild("LogScene", ImVec2(ImGui::GetWindowWidth() - 15, 120), true))
					{
						ExampleAppLog::Draw();
						ImGui::EndChild();
					}
					ImGui::PopStyleVar();
					ImGui::PopStyleColor(2);

					ImGui::EndTabItem();
				}

				/*
				* About tab
				*/
				if (ImGui::BeginTabItem("About"))
				{
					ImGui::Text("> Iolive version %s.%s", IOLIVE_MAJOR_VERSION_STR, IOLIVE_MINOR_VERSION_STR);
					ImGui::Text("> See the project on"); ImGui::SameLine();
					ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10.f);
					if (ImGui::Button("Github"))
					{
						WindowsAPI::OpenUrlInBrowser(IOLIVE_GITHUB);
					}
					ImGui::PopStyleVar();

					ImGui::EndTabItem();
				}

				ImGui::EndTabBar();

			}
			ImGui::End();
		}
		ImGui::PopStyleVar(); // ImGuiStyleVar_WindowMinSize

		if (app->m_UserModel.IsModelInitialized())
		{
			if (Checkbox_ShowModelHotkeys.IsChecked())
			{
				ShowModelHotkeys(app);

				if (doEditHotkeys > -1)
				{
					static auto HotkeysEditCallback = [](int index, ModelMotion* motion) {
						if (index > -1 && motion != nullptr) // saved
						{
							MainGui::Get().OnHotkeysSaved(index, motion);
						}

						MainGui::Get().doEditHotkeys = -1;
					};

					GuiHotkeys.DrawGuiEdit(doEditHotkeys, HotkeysEditCallback);
				}
			}
			else
			{
				doEditHotkeys = -1;
			}
		}
		EndImGuiFrame();
	}

	void MainGui::ShowModelHotkeys(Application* app)
	{
		ImGui::SetNextWindowSize(ImVec2(500, 320), ImGuiCond_Once);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(350, 224));
		{
			ImGui::Begin("Model Hotkeys", Checkbox_ShowModelHotkeys.GetPtrChecked());

			if (GuiHotkeys.Size() == 0)
			{
				float font_size = ImGui::GetFontSize() * 25 / 2;
				ImGui::SameLine( ImGui::GetWindowSize().x / 2 - font_size + (font_size / 2));
				ImGui::Text("Didn't see any motion/expression.");
			}
			else
			{
				ImGui::TextWrapped(" * Hotkeys cannot be activated globally/outside of Iolive window");

				ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.f);
				if (ImGui::Button("Reset All To Default", ImVec2(180, 28)))
				{
					app->CreateNewHotkeys((app->m_UserModel.GetModel2D()->GetModelDir() + kSettingsFileName).c_str());
				}
				ImGui::PopStyleVar();
				ImGui::NewLine();

				for (int i = 0; i < GuiHotkeys.Size(); i++)
				{
					auto item = GuiHotkeys.GetItemAtIndex(i);
					if (item != nullptr)
					{
						ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.f);
						if (ImGui::Button(item->name, ImVec2(0, 26)))
						{
							// draw edit hotkeys
							doEditHotkeys = i;
						}
						ImGui::PopStyleVar();

						HotkeyItem& hotkeyItem = GuiHotkeys.GetHotkeysAtIndex(i);
						GuiHotkeys.DrawHotkeyItem(hotkeyItem);
					}

					ImGui::NewLine();
				}
			}
			ImGui::End();
		}
		ImGui::PopStyleVar(); // Window min size
	}

	/*
	* A bridge for calling private function in Application
	*/
	void MainGui::OnHotkeysSaved(int index, ModelMotion* motion)
	{
		Application::Get()->OnHotkeysSaved(index, motion);
	}

	/*
	* Parameter Scene
	*/
	ParameterScene::ParameterScene()
		: m_Model2D(nullptr)
	{}

	void ParameterScene::SetModel(Model2D* model)
	{
		m_Model2D = model;

		for (auto [name, value] : m_Model2D->GetParameterMap())
			m_Parameters[name] = value; // copy parameter name & default value
		m_ParamMinMax = m_Model2D->GetParameterMinMax();

		// vector of pointer to m_Parameters second value
		m_ParametersPtrValue.reserve(m_Parameters.size());
		for (auto& [_key, value] : m_Parameters)
			m_ParametersPtrValue.push_back(&value);
	}

	void ParameterScene::UnsetModel()
	{
		m_Model2D = nullptr;
		ClearParameters();
	}

	void ParameterScene::Draw()
	{
		if (m_Parameters.size() < 1) return;

		ImGui::PushItemWidth(150);

		std::map<int, float*> modelBindedParameter = m_Model2D->GetBindedParameter();

		size_t paramIndex = 0;
		for (auto& [name, value] : m_Parameters)
		{
			bool isBindedWithGUI = &value == modelBindedParameter[paramIndex];
			if (!isBindedWithGUI)
			{
				// this parameter is not binded with the GUI. Disable this parameter
				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.55f);
			}

			ImGui::SliderFloat(name, modelBindedParameter[paramIndex],
				m_ParamMinMax[paramIndex][0], m_ParamMinMax[paramIndex][1], "%.2f"
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

	float* ParameterScene::GetPtrValueByIndex(int index)
	{
		return m_ParametersPtrValue[index];
	}

	int ParameterScene::GetParameterSize() const
	{
		return m_Parameters.size();
	}

	void ParameterScene::ClearParameters()
	{
		m_Parameters.clear();
		m_ParametersPtrValue.clear();
		m_ParamMinMax.clear();
	}
}