#define IMGUI_IMPL_OPENGL_LOADER_GLEW
#include "IoliveGui.hpp"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "../Window.hpp"

namespace Iolive {
	void IoliveGui::Init()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;   // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport / Platform Windows
		io.IniFilename = NULL; // don't create file .ini
		
		io.Fonts->AddFontFromFileTTF("Assets/fonts/roboto-android/Roboto-Bold.ttf", 18.0f);
		io.FontDefault = io.Fonts->AddFontFromFileTTF("Assets/fonts/roboto-android/Roboto-Regular.ttf", 18.0f);

		ImGui::StyleColorsLight();

		SetIoliveThemeColors();

		ImGui_ImplGlfw_InitForOpenGL(Window::GetWindow(), true);
		ImGui_ImplOpenGL3_Init("#version 130");

		InitWidget();
	}

	void IoliveGui::InitWidget()
	{
		LeftWidget = MainLeftWidget();
		RightWidget = MainRightWidget();
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
		LeftWidget.DoDraw();
		RightWidget.DoDraw();
	}

	void IoliveGui::SetIoliveThemeColors()
	{
		// make a good imgui style here

        /*ImGuiStyle* style = &ImGui::GetStyle();
        ImVec4* colors = style->Colors;

		colors[ImGuiCol_TitleBgActive] = ImVec4(1.0f, 179.0f / 255.0f, 179.0f / 255.0f, 1.0f);
		colors[ImGuiCol_Button] = ImVec4(0.0f, 1.0f, 1.0f, 1.0f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.0f, 1.0f, 1.0f, 0.8f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.0f, 1.0f, 1.0f, 0.9f);*/
	}
}