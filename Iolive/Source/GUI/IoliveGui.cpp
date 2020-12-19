#include "IoliveGui.hpp"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "../Application.hpp"
#include "../Window.hpp"

namespace Iolive {
	void IoliveGui::Init()
	{
		const char* glslVersion = "#version 130";

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGui::StyleColorsDark();

		ImGui_ImplGlfw_InitForOpenGL(Window::GetWindow(), true);
		ImGui_ImplOpenGL3_Init(glslVersion);

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

		DrawAllWidget();

		// render imgui
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void IoliveGui::DrawAllWidget()
	{
		LeftWidget.DoDraw();
		RightWidget.DoDraw();
	}

}