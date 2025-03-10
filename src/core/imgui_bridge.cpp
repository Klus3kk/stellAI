/**
 * @file imgui_bridge.cpp
 * @brief C++ Implementation for ImGui functions to be called from C
 */

 #include "imgui/imgui.h"
 #include "imgui/imgui_impl_glfw.h"
 #include "imgui/imgui_impl_opengl3.h"
 #include <glad/glad.h>
 #include <GLFW/glfw3.h>
 #include <string>
 #include <vector>
 #include <iostream>
 
 // StellAI integration
 #include "StellAI.hpp"
 #include "StellAI_GUI.hpp"
 
 // Export functions for C code
 extern "C" {
 
 void imgui_init(GLFWwindow* window) {
     // Setup Dear ImGui context
     IMGUI_CHECKVERSION();
     ImGui::CreateContext();
     ImGuiIO& io = ImGui::GetIO();
     io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
     io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
     io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // Enable Docking
     io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;   // Enable Multi-Viewport
 
     // Setup ImGui style
     ImGui::StyleColorsDark();
 
     // When viewports are enabled, we tweak WindowRounding/WindowBg so platform windows can look identical
     ImGuiStyle& style = ImGui::GetStyle();
     if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
         style.WindowRounding = 0.0f;
         style.Colors[ImGuiCol_WindowBg].w = 1.0f;
     }
 
     // Setup Platform/Renderer backends
     ImGui_ImplGlfw_InitForOpenGL(window, true);
     ImGui_ImplOpenGL3_Init("#version 330");
 
     // Initialize fonts
     io.Fonts->AddFontDefault();
 }
 
 void imgui_shutdown() {
     ImGui_ImplOpenGL3_Shutdown();
     ImGui_ImplGlfw_Shutdown();
     ImGui::DestroyContext();
 }
 
 void imgui_new_frame() {
     ImGui_ImplOpenGL3_NewFrame();
     ImGui_ImplGlfw_NewFrame();
     ImGui::NewFrame();
 }
 
 void imgui_render() {
     ImGui::Render();
     ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
 
     // Update and Render additional Platform Windows
     ImGuiIO& io = ImGui::GetIO();
     if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
         GLFWwindow* backup_current_context = glfwGetCurrentContext();
         ImGui::UpdatePlatformWindows();
         ImGui::RenderPlatformWindowsDefault();
         glfwMakeContextCurrent(backup_current_context);
     }
 }
 
 void imgui_set_dark_theme() {
     ImGui::StyleColorsDark();
     
     // Add more custom styling for dark theme
     ImGuiStyle& style = ImGui::GetStyle();
     
     // Dark blue / purple sci-fi theme
     style.Colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.17f, 1.00f);
     style.Colors[ImGuiCol_Header] = ImVec4(0.20f, 0.22f, 0.32f, 1.00f);
     style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.28f, 0.40f, 1.00f);
     style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.30f, 0.32f, 0.45f, 1.00f);
     style.Colors[ImGuiCol_Button] = ImVec4(0.25f, 0.27f, 0.38f, 1.00f);
     style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.32f, 0.34f, 0.45f, 1.00f);
     style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.39f, 0.41f, 0.52f, 1.00f);
     style.Colors[ImGuiCol_FrameBg] = ImVec4(0.15f, 0.16f, 0.22f, 1.00f);
     style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.25f, 0.26f, 0.33f, 1.00f);
     style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.30f, 0.31f, 0.38f, 1.00f);
     style.Colors[ImGuiCol_Tab] = ImVec4(0.20f, 0.22f, 0.32f, 1.00f);
     style.Colors[ImGuiCol_TabHovered] = ImVec4(0.26f, 0.28f, 0.40f, 1.00f);
     style.Colors[ImGuiCol_TabActive] = ImVec4(0.30f, 0.32f, 0.45f, 1.00f);
     style.Colors[ImGuiCol_TitleBg] = ImVec4(0.13f, 0.14f, 0.20f, 1.00f);
     style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.20f, 0.22f, 0.32f, 1.00f);
     style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.15f, 0.16f, 0.22f, 1.00f);
     
     // Set rounded corners for a more modern look
     style.WindowRounding = 5.0f;
     style.FrameRounding = 4.0f;
     style.PopupRounding = 4.0f;
     style.ScrollbarRounding = 4.0f;
     style.GrabRounding = 4.0f;
     style.TabRounding = 4.0f;
 }
 
 void imgui_set_light_theme() {
     ImGui::StyleColorsLight();
     
     // Add more custom styling for light theme
     ImGuiStyle& style = ImGui::GetStyle();
     
     // Soft blue light theme
     style.Colors[ImGuiCol_WindowBg] = ImVec4(0.94f, 0.94f, 0.97f, 1.00f);
     style.Colors[ImGuiCol_Header] = ImVec4(0.70f, 0.75f, 0.85f, 1.00f);
     style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.75f, 0.80f, 0.90f, 1.00f);
     style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.80f, 0.85f, 0.95f, 1.00f);
     style.Colors[ImGuiCol_Button] = ImVec4(0.75f, 0.80f, 0.90f, 1.00f);
     style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.80f, 0.85f, 0.95f, 1.00f);
     style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.85f, 0.90f, 1.00f, 1.00f);
     style.Colors[ImGuiCol_FrameBg] = ImVec4(0.85f, 0.85f, 0.90f, 1.00f);
     style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.90f, 0.90f, 0.95f, 1.00f);
     style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.95f, 0.95f, 1.00f, 1.00f);
     
     // Set rounded corners for a more modern look
     style.WindowRounding = 5.0f;
     style.FrameRounding = 4.0f;
     style.PopupRounding = 4.0f;
     style.ScrollbarRounding = 4.0f;
     style.GrabRounding = 4.0f;
     style.TabRounding = 4.0f;
 }
 
 bool imgui_begin_window(const char* name, bool* p_open, int flags) {
     ImGuiWindowFlags window_flags = 0;
     
     // Convert C-style flags to ImGui flags
     if (flags & 0x01) window_flags |= ImGuiWindowFlags_NoTitleBar;
     if (flags & 0x02) window_flags |= ImGuiWindowFlags_NoResize;
     if (flags & 0x04) window_flags |= ImGuiWindowFlags_NoMove;
     if (flags & 0x08) window_flags |= ImGuiWindowFlags_NoScrollbar;
     if (flags & 0x10) window_flags |= ImGuiWindowFlags_NoScrollWithMouse;
     if (flags & 0x20) window_flags |= ImGuiWindowFlags_NoCollapse;
     
     return ImGui::Begin(name, p_open, window_flags);
 }
 
 void imgui_end_window() {
     ImGui::End();
 }
 
 bool imgui_button(const char* label, float width, float height) {
     if (width <= 0 && height <= 0) {
         return ImGui::Button(label);
     } else {
         return ImGui::Button(label, ImVec2(width, height));
     }
 }
 
 bool imgui_begin_main_menu_bar() {
     return ImGui::BeginMainMenuBar();
 }
 
 void imgui_end_main_menu_bar() {
     ImGui::EndMainMenuBar();
 }
 
 bool imgui_begin_menu(const char* label, bool enabled) {
     return ImGui::BeginMenu(label, enabled);
 }
 
 void imgui_end_menu() {
     ImGui::EndMenu();
 }
 
 bool imgui_menu_item(const char* label, const char* shortcut, bool selected, bool enabled) {
     return ImGui::MenuItem(label, shortcut, selected, enabled);
 }
 
 void imgui_text(const char* text) {
     ImGui::Text("%s", text);
 }
 
 void imgui_input_text(const char* label, char* buf, size_t buf_size) {
     ImGui::InputText(label, buf, buf_size);
 }
 
 bool imgui_color_picker3(const char* label, float col[3]) {
     return ImGui::ColorEdit3(label, col);
 }
 
 bool imgui_slider_float(const char* label, float* v, float v_min, float v_max) {
     return ImGui::SliderFloat(label, v, v_min, v_max);
 }
 
 bool imgui_checkbox(const char* label, bool* v) {
     return ImGui::Checkbox(label, v);
 }
 
 void imgui_separator() {
     ImGui::Separator();
 }
 
 void imgui_same_line() {
     ImGui::SameLine();
 }
 
 bool imgui_collapsing_header(const char* label, int flags) {
     ImGuiTreeNodeFlags tree_flags = 0;
     
     // Convert C-style flags to ImGui flags
     if (flags & 0x01) tree_flags |= ImGuiTreeNodeFlags_DefaultOpen;
     if (flags & 0x02) tree_flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
     if (flags & 0x04) tree_flags |= ImGuiTreeNodeFlags_OpenOnArrow;
     
     return ImGui::CollapsingHeader(label, tree_flags);
 }
 
 bool imgui_tree_node(const char* label) {
     return ImGui::TreeNode(label);
 }
 
 void imgui_tree_pop() {
     ImGui::TreePop();
 }
 
 void imgui_columns(int count, const char* id, bool border) {
     ImGui::Columns(count, id, border);
 }
 
 void imgui_next_column() {
     ImGui::NextColumn();
 }
 
 void imgui_begin_child(const char* str_id, float width, float height, bool border) {
     ImGui::BeginChild(str_id, ImVec2(width, height), border);
 }
 
 void imgui_end_child() {
     ImGui::EndChild();
 }
 
 void imgui_set_item_default_focus() {
     ImGui::SetItemDefaultFocus();
 }
 
 bool imgui_is_item_focused() {
     return ImGui::IsItemFocused();
 }
 
 void imgui_show_demo_window(bool* p_open) {
     ImGui::ShowDemoWindow(p_open);
 }
 
 // StellAI integration functions
 
 void stellai_initialize() {
     try {
         StellAI::Engine::getInstance().initialize(true);
         StellAI::GUI::StellAIGUI::getInstance().initialize(glfwGetCurrentContext());
         std::cout << "StellAI initialized successfully" << std::endl;
     } catch (const std::exception& e) {
         std::cerr << "Failed to initialize StellAI: " << e.what() << std::endl;
     }
 }
 
 void stellai_shutdown() {
     try {
         StellAI::GUI::StellAIGUI::getInstance().shutdown();
         StellAI::Engine::getInstance().shutdown();
         std::cout << "StellAI shut down successfully" << std::endl;
     } catch (const std::exception& e) {
         std::cerr << "Error during StellAI shutdown: " << e.what() << std::endl;
     }
 }
 
 bool stellai_is_initialized() {
     return StellAI::GUI::StellAIGUI::getInstance().isInitialized();
 }
 
 bool stellai_is_enabled() {
     // In a full implementation, this would check if AI features are enabled
     return true;
 }
 
 void stellai_show_main_window(bool show) {
     StellAI::GUI::StellAIGUI::getInstance().setShowMainWindow(show);
 }
 
 void stellai_toggle_main_window() {
     StellAI::GUI::StellAIGUI::getInstance().toggleMainWindow();
 }
 
 void stellai_update(float deltaTime) {
     StellAI::GUI::StellAIGUI::getInstance().update(deltaTime);
 }
 
 void stellai_render() {
     StellAI::GUI::StellAIGUI::getInstance().render();
 }
 
 } // extern "C"