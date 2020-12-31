#define IMGUI_IMPL_OPENGL_LOADER_GLEW
#include "IoliveGui.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

// Widget
#include "Widget/MainWidget.hpp"

// Access parent function
#include "../Window.hpp"

namespace Iolive {
	void IoliveGui::Init()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;   // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport / Platform Windows
		io.ConfigViewportsNoAutoMerge = true; // fly... me to the moon~
		io.IniFilename = NULL; // don't create file .ini
		
		io.Fonts->AddFontFromFileTTF("Assets/fonts/roboto-android/Roboto-Bold.ttf", 18.0f);
		io.FontDefault = io.Fonts->AddFontFromFileTTF("Assets/fonts/roboto-android/Roboto-Regular.ttf", 18.0f);

		ImGui::StyleColorsLight();

		SetupGuiTheme();

		ImGui_ImplGlfw_InitForOpenGL(Window::GetWindow(), true);
		ImGui_ImplOpenGL3_Init("#version 130");
	}

	void IoliveGui::Shutdown()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void IoliveGui::OnDraw()
	{
		// starts new imgui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		int width, height;
		Window::GetWindowSize(&width, &height);
		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2(width, height);

		// draw our widget
		DrawAllWidget();

		// rendering
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
	}

	void IoliveGui::DrawAllWidget()
	{
		MainWidget::DoDraw();
	}

	void IoliveGui::SetupGuiTheme()
	{
		// make a good imgui style here
		
		ImGuiStyle& style = ImGui::GetStyle();

		struct ImVec3 { float x, y, z; ImVec3(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f) { x = _x; y = _y; z = _z; } };

		static ImVec3 color_for_head = ImVec3(245.f / 255.f, 144.f / 255.f, 158.f / 255.f);
		static ImVec3 color_for_area = ImVec3(238.f / 255.f, 220.f / 255.f, 215.f / 255.f);
		static ImVec3 color_for_body = ImVec3(254.f / 255.f, 254.f / 255.f, 254.f / 255.f);
		static ImVec3 color_for_tab = ImVec3(198.f / 255.f, 235.f / 255.f, 105.f / 255.f);
		static ImVec3 color_for_text = ImVec3(29.f / 255.f, 39.f / 255.f, 53.f / 255.f);

		style.Colors[ImGuiCol_CheckMark] = ImVec4(0.05f, 0.80f, 0.05f, 1.0f);
		style.Colors[ImGuiCol_Text] = ImVec4(color_for_text.x, color_for_text.y, color_for_text.z, 1.0f);
		style.Colors[ImGuiCol_TextDisabled] = ImVec4(color_for_text.x, color_for_text.y, color_for_text.z, 0.55f);
		style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 0.50f);
		style.Colors[ImGuiCol_WindowBg] = ImVec4(color_for_body.x, color_for_body.y, color_for_body.z, 0.95f);
		style.Colors[ImGuiCol_Border] = ImVec4(color_for_body.x, color_for_body.y, color_for_body.z, 0.00f);
		style.Colors[ImGuiCol_BorderShadow] = ImVec4(color_for_body.x, color_for_body.y, color_for_body.z, 0.00f);
		style.Colors[ImGuiCol_FrameBg] = ImVec4(color_for_area.x, color_for_area.y, color_for_area.z, 1.0f);
		style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 0.7f);
		style.Colors[ImGuiCol_FrameBgActive] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 0.86f);
		style.Colors[ImGuiCol_TitleBg] = ImVec4(color_for_area.x, color_for_area.y, color_for_area.z, 1.00f);
		style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(color_for_area.x, color_for_area.y, color_for_area.z, 0.75f);
		style.Colors[ImGuiCol_TitleBgActive] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 1.00f);
		style.Colors[ImGuiCol_MenuBarBg] = ImVec4(color_for_area.x, color_for_area.y, color_for_area.z, 0.47f);
		style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(color_for_area.x, color_for_area.y, color_for_area.z, 1.00f);
		style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 0.5f);
		style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 0.75f);
		style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 1.00f);
		style.Colors[ImGuiCol_SliderGrab] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 0.75f);
		style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 1.00f);
		style.Colors[ImGuiCol_Button] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 0.75f);
		style.Colors[ImGuiCol_ButtonHovered] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 0.86f);
		style.Colors[ImGuiCol_ButtonActive] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 1.00f);
		style.Colors[ImGuiCol_Header] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 0.76f);
		style.Colors[ImGuiCol_HeaderHovered] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 0.86f);
		style.Colors[ImGuiCol_HeaderActive] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 1.00f);
		style.Colors[ImGuiCol_ResizeGrip] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 0.90f);
		style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 0.75f);
		style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 1.00f);
		style.Colors[ImGuiCol_Tab] = ImVec4(color_for_tab.x, color_for_tab.y, color_for_tab.z, 1.0f);
		style.Colors[ImGuiCol_TabHovered] = ImVec4(color_for_tab.x, color_for_tab.y, color_for_tab.z, 0.65f);
		style.Colors[ImGuiCol_TabActive] = ImVec4(color_for_tab.x, color_for_tab.y, color_for_tab.z, 0.65f);
		style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(color_for_area.x, color_for_area.y, color_for_area.z, 0.73f);
	}
} // namespace Iolive