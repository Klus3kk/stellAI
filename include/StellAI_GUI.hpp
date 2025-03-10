/**
 * @file StellAI_GUI.hpp
 * @brief GUI interface for StellAI, using ImGui
*/

 #ifndef STELLAI_GUI_HPP
 #define STELLAI_GUI_HPP
 
 #include <string>
 #include <vector>
 #include <memory>
 #include <cstring>
 #include "StellAI.hpp"
 
 // Forward declarations
 struct GLFWwindow;
 
 namespace StellAI {
 namespace GUI {
 
 /**
  * Main GUI class for StellAI
  */
 class StellAIGUI {
 private:
     bool initialized;
     bool showMainWindow;
     bool showTerrainGenerator;
     bool showModelGenerator;
     bool showMaterialGenerator;
     bool showShaderGenerator;
     bool showAISettings;
     bool showHelpWindow;
     bool enableAI;
     
     // Generation parameters
     WorldGen::TerrainParams terrainParams;
     ModelGen::ModelGenParams modelParams;
     ShaderGen::ShaderGenParams shaderParams;
     
     // Text buffers for UI input
     char modelPromptBuffer[256];
     char shaderEffectBuffer[256];
     
     // Private constructor for singleton
     StellAIGUI();
     
     // Render individual windows
     void renderMainWindow();
     void renderTerrainGeneratorWindow();
     void renderModelGeneratorWindow();
     void renderMaterialGeneratorWindow();
     void renderShaderGeneratorWindow();
     void renderAISettingsWindow();
     void renderHelpWindow();
     
 public:
     // Delete copy constructor and assignment operator
     StellAIGUI(const StellAIGUI&) = delete;
     StellAIGUI& operator=(const StellAIGUI&) = delete;
     
     /**
      * Get the singleton instance
      */
     static StellAIGUI& getInstance() {
         static StellAIGUI instance;
         return instance;
     }
     
     /**
      * Initialize the GUI
      * @param window GLFW window to use
      * @return True if initialization succeeded
      */
     bool initialize(GLFWwindow* window);
     
     /**
      * Update the GUI
      * @param deltaTime Time since last frame
      */
     void update(float deltaTime);
     
     /**
      * Render the GUI
      */
     void render();
     
     /**
      * Shutdown the GUI and free resources
      */
     void shutdown();
     
     /**
      * Check if the GUI is initialized
      */
     bool isInitialized() const { return initialized; }
     
     /**
      * Set whether to show the main window
      */
     void setShowMainWindow(bool show) { showMainWindow = show; }
     
     /**
      * Toggle the main window visibility
      */
     void toggleMainWindow() { showMainWindow = !showMainWindow; }
 };
 
 } // namespace GUI
 } // namespace StellAI
 
 #endif /* STELLAI_GUI_HPP */