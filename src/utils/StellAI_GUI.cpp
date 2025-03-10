/**
 * @file StellAI_GUI.cpp
 * @brief Implementation of ImGui-based GUI for StellAI
 */

 #include "StellAI_GUI.hpp"
 #include "StellAI.hpp"
 #include "imgui/imgui.h"
 #include "imgui/imgui_internal.h"
 #include <string>
 #include <vector>
 #include <algorithm>
 #include <iostream>
 
 // C-style headers for ClueEngine integration
 extern "C" {
     #include "materials.h"
     #include "ModelLoad.h"
     #include "globals.h"
     #include "ObjectManager.h"
     #include "gui.h"
 }
 
 namespace StellAI {
 namespace GUI {
 
 StellAIGUI::StellAIGUI() 
     : initialized(false),
       showMainWindow(false),
       showTerrainGenerator(false),
       showModelGenerator(false),
       showMaterialGenerator(false),
       showShaderGenerator(false),
       showAISettings(false),
       showHelpWindow(false),
       enableAI(true)
 {
     // Initialize terrain params with default values
     terrainParams.scale = 1.0f;
     terrainParams.roughness = 0.5f;
     terrainParams.amplitude = 10.0f;
     terrainParams.octaves = 4;
     terrainParams.seed = 12345;
     terrainParams.position = {0.0f, 0.0f, 0.0f};
 
     // Initialize model params
     strcpy(modelPromptBuffer, "cube");
     modelParams.prompt = modelPromptBuffer;
     modelParams.complexity = 0.5f;
     modelParams.resolution = 32;
     modelParams.texturing = true;
     modelParams.size = {1.0f, 1.0f, 1.0f};
 
     // Initialize shader params
     strcpy(shaderEffectBuffer, "normal");
     shaderParams.effect = shaderEffectBuffer;
     shaderParams.optimizeForPerformance = false;
 }
 
 bool StellAIGUI::initialize(GLFWwindow* window) {
     if (initialized) {
         return true;
     }
 
     // StellAI is already initialized in the ImGui bridge
     initialized = true;
     return true;
 }
 
 void StellAIGUI::update(float deltaTime) {
     if (!initialized || !showMainWindow) {
         return;
     }
 
     // Render StellAI windows
     renderMainWindow();
 
     if (showTerrainGenerator) {
         renderTerrainGeneratorWindow();
     }
 
     if (showModelGenerator) {
         renderModelGeneratorWindow();
     }
 
     if (showMaterialGenerator) {
         renderMaterialGeneratorWindow();
     }
 
     if (showShaderGenerator) {
         renderShaderGeneratorWindow();
     }
 
     if (showAISettings) {
         renderAISettingsWindow();
     }
 
     if (showHelpWindow) {
         renderHelpWindow();
     }
 }
 
 void StellAIGUI::render() {
     // Nothing to do here, rendering is handled in update
 }
 
 void StellAIGUI::shutdown() {
     initialized = false;
 }
 
 void StellAIGUI::renderMainWindow() {
     ImGui::SetNextWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);
     ImGui::SetNextWindowPos(ImVec2(50, 50), ImGuiCond_FirstUseEver);
     
     // Create the StellAI main window
     if (ImGui::Begin("StellAI - AI-Enhanced 3D Engine", &showMainWindow, 
                     ImGuiWindowFlags_MenuBar)) {
         
         // Menu bar
         if (ImGui::BeginMenuBar()) {
             if (ImGui::BeginMenu("File")) {
                 if (ImGui::MenuItem("New Project")) {
                     // TODO: Implement new project functionality
                 }
                 if (ImGui::MenuItem("Open Project...")) {
                     // TODO: Implement open project functionality
                 }
                 if (ImGui::MenuItem("Save Project")) {
                     // TODO: Implement save project functionality
                 }
                 ImGui::Separator();
                 if (ImGui::MenuItem("Close StellAI")) {
                     showMainWindow = false;
                 }
                 ImGui::EndMenu();
             }
             
             if (ImGui::BeginMenu("Tools")) {
                 if (ImGui::MenuItem("Terrain Generator", nullptr, showTerrainGenerator)) {
                     showTerrainGenerator = !showTerrainGenerator;
                 }
                 if (ImGui::MenuItem("Model Generator", nullptr, showModelGenerator)) {
                     showModelGenerator = !showModelGenerator;
                 }
                 if (ImGui::MenuItem("Material Generator", nullptr, showMaterialGenerator)) {
                     showMaterialGenerator = !showMaterialGenerator;
                 }
                 if (ImGui::MenuItem("Shader Generator", nullptr, showShaderGenerator)) {
                     showShaderGenerator = !showShaderGenerator;
                 }
                 ImGui::EndMenu();
             }
             
             if (ImGui::BeginMenu("Settings")) {
                 if (ImGui::MenuItem("AI Settings", nullptr, showAISettings)) {
                     showAISettings = !showAISettings;
                 }
                 ImGui::EndMenu();
             }
             
             if (ImGui::BeginMenu("Help")) {
                 if (ImGui::MenuItem("StellAI Help", nullptr, showHelpWindow)) {
                     showHelpWindow = !showHelpWindow;
                 }
                 if (ImGui::MenuItem("About...")) {
                     ImGui::OpenPopup("About StellAI");
                 }
                 ImGui::EndMenu();
             }
             
             ImGui::EndMenuBar();
         }
         
         // About popup
         if (ImGui::BeginPopupModal("About StellAI", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
             ImGui::Text("StellAI v%s", Version::toString().c_str());
             ImGui::Separator();
             ImGui::Text("An AI-powered extension for ClueEngine");
             ImGui::Text("Developed by: Your Name");
             ImGui::Text("© 2024 All rights reserved");
             
             ImGui::Separator();
             if (ImGui::Button("OK", ImVec2(120, 0))) {
                 ImGui::CloseCurrentPopup();
             }
             ImGui::EndPopup();
         }
         // Logo and version
         ImGui::SeparatorText("StellAI v" + Version::toString());
         
         // Main tabs
         static int selected_tab = 0;
         const char* tabs[] = { "Overview", "AI Tools", "Settings", "Help" };
         
         ImGui::BeginTabBar("StellAITabs");
         
         // Overview tab
         if (ImGui::BeginTabItem(tabs[0])) {
             ImGui::Text("Welcome to StellAI!");
             ImGui::Text("StellAI extends ClueEngine with AI-powered capabilities for procedural generation,");
             ImGui::Text("intelligent texturing, and custom shader creation.");
             
             ImGui::Spacing();
             ImGui::Separator();
             ImGui::Spacing();
             
             ImGui::Text("AI Status: %s", enableAI ? "Enabled" : "Disabled");
             
             ImGui::EndTabItem();
         }
         
         // AI Tools tab
         if (ImGui::BeginTabItem(tabs[1])) {
             ImGui::Spacing();
             
             // Tool Selection
             if (ImGui::CollapsingHeader("Generation Tools", ImGuiTreeNodeFlags_DefaultOpen)) {
                 ImGui::Indent(10.0f);
                 
                 // Terrain Generator
                 if (ImGui::Button("Terrain Generator", ImVec2(180, 40))) {
                     showTerrainGenerator = true;
                 }
                 ImGui::SameLine();
                 ImGui::TextDisabled("(?)");
                 if (ImGui::IsItemHovered()) {
                     ImGui::BeginTooltip();
                     ImGui::Text("Generate procedural terrain using AI");
                     ImGui::EndTooltip();
                 }
                 
                 ImGui::Spacing();
                 
                 // Model Generator
                 if (ImGui::Button("Model Generator", ImVec2(180, 40))) {
                     showModelGenerator = true;
                 }
                 ImGui::SameLine();
                 ImGui::TextDisabled("(?)");
                 if (ImGui::IsItemHovered()) {
                     ImGui::BeginTooltip();
                     ImGui::Text("Generate 3D models from text descriptions using AI");
                     ImGui::EndTooltip();
                 }
                 
                 ImGui::Spacing();
                 
                 // Material Generator
                 if (ImGui::Button("Material Generator", ImVec2(180, 40))) {
                     showMaterialGenerator = true;
                 }
                 ImGui::SameLine();
                 ImGui::TextDisabled("(?)");
                 if (ImGui::IsItemHovered()) {
                     ImGui::BeginTooltip();
                     ImGui::Text("Generate PBR materials using AI");
                     ImGui::EndTooltip();
                 }
                 
                 ImGui::Spacing();
                 
                 // Shader Generator
                 if (ImGui::Button("Shader Generator", ImVec2(180, 40))) {
                     showShaderGenerator = true;
                 }
                 ImGui::SameLine();
                 ImGui::TextDisabled("(?)");
                 if (ImGui::IsItemHovered()) {
                     ImGui::BeginTooltip();
                     ImGui::Text("Generate custom shaders using AI");
                     ImGui::EndTooltip();
                 }
                 
                 ImGui::Unindent(10.0f);
             }
             
             ImGui::EndTabItem();
         }
         
         // Settings tab
         if (ImGui::BeginTabItem(tabs[2])) {
             ImGui::Spacing();
             
             ImGui::Checkbox("Enable AI Features", &enableAI);
             
             ImGui::Separator();
             
             // Performance settings
             if (ImGui::CollapsingHeader("Performance Settings")) {
                 static int ai_quality = 1;
                 ImGui::Text("AI Quality:");
                 ImGui::RadioButton("Low (Fast)", &ai_quality, 0);
                 ImGui::RadioButton("Medium (Balanced)", &ai_quality, 1);
                 ImGui::RadioButton("High (Best Results)", &ai_quality, 2);
                 
                 ImGui::Separator();
                 
                 static bool use_gpu_acceleration = true;
                 ImGui::Checkbox("Use GPU Acceleration (if available)", &use_gpu_acceleration);
                 
                 ImGui::Separator();
                 
                 static int cache_size = 2;
                 const char* cache_options[] = { "Small (512MB)", "Medium (1GB)", "Large (2GB)" };
                 ImGui::Text("AI Model Cache Size:");
                 ImGui::Combo("##cache_size", &cache_size, cache_options, IM_ARRAYSIZE(cache_options));
             }
             
             ImGui::EndTabItem();
         }
         
         // Help tab
         if (ImGui::BeginTabItem(tabs[3])) {
             ImGui::Text("StellAI Help");
             ImGui::Separator();
             
             if (ImGui::CollapsingHeader("Using the Terrain Generator")) {
                 ImGui::BulletText("Use the Terrain Generator to create procedural landscapes");
                 ImGui::BulletText("Adjust parameters like scale, roughness, and amplitude");
                 ImGui::BulletText("AI will analyze your terrain and apply appropriate biomes");
                 ImGui::BulletText("Generated terrain will be added to your scene automatically");
             }
             
             if (ImGui::CollapsingHeader("Using the Model Generator")) {
                 ImGui::BulletText("Enter a text description of what you want to create");
                 ImGui::BulletText("Adjust complexity and resolution to control detail level");
                 ImGui::BulletText("AI will generate a 3D model based on your description");
                 ImGui::BulletText("You can further refine the model with additional prompts");
             }
             
             if (ImGui::CollapsingHeader("Using the Material Generator")) {
                 ImGui::BulletText("AI can generate PBR materials for your models");
                 ImGui::BulletText("Describe the material (e.g., 'rusty metal', 'polished wood')");
                 ImGui::BulletText("All PBR maps will be generated: albedo, normal, metallic, roughness, AO");
                 ImGui::BulletText("Materials can be saved for future use in your project");
             }
             
             if (ImGui::CollapsingHeader("Using the Shader Generator")) {
                 ImGui::BulletText("Create custom shaders with simple text descriptions");
                 ImGui::BulletText("Specify visual effects, optimizations, and features");
                 ImGui::BulletText("AI will generate both vertex and fragment shaders");
                 ImGui::BulletText("Shaders can be edited and refined with additional prompts");
             }
             
             ImGui::EndTabItem();
         }
         
         ImGui::EndTabBar();
     }
     ImGui::End();
 }