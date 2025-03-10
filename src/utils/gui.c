// Include necessary headers
#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <stdbool.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Assimp headers
#include <assimp/scene.h>
#include <assimp/vector3.h>
#include <assimp/cimport.h>
#include <assimp/postprocess.h>

// ImGui headers - we'll use C++ interface via extern "C" in the implementation
#ifdef __cplusplus
extern "C" {
#endif

// Forward declare ImGui C++ functions that we'll implement in a separate C++ file
void imgui_init(GLFWwindow* window);
void imgui_shutdown();
void imgui_new_frame();
void imgui_render();
void imgui_set_dark_theme();
void imgui_set_light_theme();
bool imgui_begin_window(const char* name, bool* p_open, int flags);
void imgui_end_window();
bool imgui_button(const char* label, float width, float height);
bool imgui_begin_main_menu_bar();
void imgui_end_main_menu_bar();
bool imgui_begin_menu(const char* label, bool enabled);
void imgui_end_menu();
bool imgui_menu_item(const char* label, const char* shortcut, bool selected, bool enabled);
void imgui_text(const char* text);
void imgui_input_text(const char* label, char* buf, size_t buf_size);
bool imgui_color_picker3(const char* label, float col[3]);
bool imgui_slider_float(const char* label, float* v, float v_min, float v_max);
bool imgui_checkbox(const char* label, bool* v);
void imgui_separator();
void imgui_same_line();
bool imgui_collapsing_header(const char* label, int flags);
bool imgui_tree_node(const char* label);
void imgui_tree_pop();
void imgui_columns(int count, const char* id, bool border);
void imgui_next_column();
void imgui_begin_child(const char* str_id, float width, float height, bool border);
void imgui_end_child();
void imgui_set_item_default_focus();
bool imgui_is_item_focused();
void imgui_show_demo_window(bool* p_open);

// StellAI functions - these will be implemented in a C++ file
void stellai_initialize();
void stellai_shutdown();
bool stellai_is_initialized();
bool stellai_is_enabled();
void stellai_show_main_window(bool show);
void stellai_toggle_main_window();
void stellai_update(float deltaTime);
void stellai_render();

#ifdef __cplusplus
}
#endif

// Project headers
#include "globals.h"
#include "gui.h"
#include "ObjectManager.h"
#include "resource_loader.h"
#include "rendering.h"
#include "textures.h"
#include "lightshading.h"
#include "file_operations.h"
#include "background.h"
#include "actions.h"

extern int textureCount;
extern int materialCount;

static bool show_material_window = false;
static bool show_texture_window = false;
static SceneObject* material_window_obj = NULL;
static SceneObject* texture_window_obj = NULL;
extern SceneObject* selected_object;

SceneObject* clipboard_object = NULL; // Clipboard for cut/copy/paste
GLuint textureColorbuffer; // External linkage to the texture from rendering.c
GLFWwindow* window;
bool theme_dark = true;
bool ctrlPressed = false;
bool show_about = false;
bool show_debug = false;
bool show_hierarchy = true;
bool show_inspector = true; 
bool show_history = false;
bool show_settings = false; 
bool isCutOperation = false;
bool show_controls = false;
bool show_object_creator = false;
bool show_color_picker = false;
bool show_change_background = false;
bool show_stellai = false;

bool is_fullscreen = false;
int windowed_x = 100, windowed_y = 100, windowed_width = 1920, windowed_height = 1080;

// Function to find the selected object index
int find_selected_object_index(SceneObject* selected_object) {
    for (int i = 0; i < objectManager.count; i++) {
        if (&objectManager.objects[i] == selected_object) {
            return i;
        }
    }
    return -1; // Not found
}

// Change texture function
void change_texture(SceneObject* obj) {
    if (obj == NULL) return;
    
    // This will be converted to use ImGui in the UI rendering function
    show_texture_window = true;
    texture_window_obj = obj;
}

// Import model function
void import_model() {
    char const* filterPatterns[1] = { "*.obj" };
    char const* filePath = tinyfd_openFileDialog("Import Model", "", 1, filterPatterns, "Object Files", 0);

    if (!filePath) {
        fprintf(stderr, "Import operation cancelled or failed to get a valid path.\n");
        return;
    }

    Model* model = loadModel(filePath);
    if (model) {
        PBRMaterial defaultMaterial = { 0 };
        addObjectWithAction(OBJ_MODEL, false, -1, true, model, defaultMaterial, false);
    }
}

void cut_object() {
    if (selected_object) {
        int index = find_selected_object_index(selected_object);
        if (index != -1) {
            clipboard_object = (SceneObject*)malloc(sizeof(SceneObject));
            if (clipboard_object) {
                *clipboard_object = *selected_object;
                isCutOperation = true;
                removeObjectWithAction(index);
                selected_object = NULL;
                printf("Cut object at index: %d\n", index);
            }
        }
    }
}

void copy_object() {
    if (selected_object) {
        clipboard_object = (SceneObject*)malloc(sizeof(SceneObject));
        if (clipboard_object) {
            *clipboard_object = *selected_object;
            if (selected_object->object.type == OBJ_MODEL) {
                clipboard_object->object.data.model.meshes = (Mesh*)malloc(selected_object->object.data.model.meshCount * sizeof(Mesh));
                for (unsigned int i = 0; i < selected_object->object.data.model.meshCount; i++) {
                    clipboard_object->object.data.model.meshes[i] = selected_object->object.data.model.meshes[i];
                }
            }
            isCutOperation = false;
        }
    }
}

void paste_object() {
    if (clipboard_object) {
        SceneObject newObject = *clipboard_object;
        if (clipboard_object->object.type == OBJ_MODEL) {
            newObject.object.data.model.meshes = (Mesh*)malloc(clipboard_object->object.data.model.meshCount * sizeof(Mesh));
            for (unsigned int i = 0; i < clipboard_object->object.data.model.meshCount; i++) {
                newObject.object.data.model.meshes[i] = clipboard_object->object.data.model.meshes[i];
            }
        }

        if (isCutOperation) {
            addObjectWithAction(newObject.object.type, newObject.object.useTexture, newObject.object.textureID, newObject.object.useColor,
                (newObject.object.type == OBJ_MODEL ? &newObject.object.data.model : NULL), newObject.object.material, newObject.object.usePBR);
            free(clipboard_object);
            clipboard_object = NULL;
            isCutOperation = false;
        }
        else {
            addObjectWithAction(newObject.object.type, newObject.object.useTexture, newObject.object.textureID, newObject.object.useColor,
                (newObject.object.type == OBJ_MODEL ? &newObject.object.data.model : NULL), newObject.object.material, newObject.object.usePBR);
        }
        selected_object = &objectManager.objects[objectManager.count - 1];
    }
}

void close_object_windows() {
    show_material_window = false;
    show_texture_window = false;
    show_color_picker = false;
}

// Setup ImGui
void setup_imgui(GLFWwindow* existingWindow) {
    window = existingWindow;
    imgui_init(window);
    set_theme(theme_dark);
    
    // Initialize StellAI if available
    initialize_stellai();
}

// Set theme function
void set_theme(bool dark_theme) {
    theme_dark = dark_theme;
    if (dark_theme) {
        imgui_set_dark_theme();
    } else {
        imgui_set_light_theme();
    }
}

// Toggle theme function
void toggle_theme() {
    set_theme(!theme_dark);
}

// Framebuffer size callback
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    windowed_width = width;
    windowed_height = height;
}

// Toggle fullscreen function
void toggle_fullscreen(GLFWwindow* window) {
    static int windowed_xpos, windowed_ypos, windowed_width, windowed_height;
    static int is_fullscreen = 0;

    if (is_fullscreen) {
        glfwSetWindowMonitor(window, NULL, windowed_xpos, windowed_ypos, windowed_width, windowed_height, 0);
        is_fullscreen = 0;
    }
    else {
        glfwGetWindowPos(window, &windowed_xpos, &windowed_ypos);
        glfwGetWindowSize(window, &windowed_width, &windowed_height);

        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
        is_fullscreen = 1;
    }
}

// Resize callback
void resize_callback(GLFWwindow* window, int width, int height) {
    printf("Resizing: width=%d, height=%d\n", width, height);
    windowed_width = width;
    windowed_height = height;
    glViewport(0, 0, width, height);
}

// Key callback function
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_F && action == GLFW_PRESS) {
        toggle_fullscreen(window);
    }

    if (key == GLFW_KEY_E && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);  
    }

    if (key == GLFW_KEY_P && action == GLFW_PRESS) {
        isRunning = !isRunning; 
    }
    
    if (key == GLFW_KEY_F1 && action == GLFW_PRESS) {
        toggle_theme();
    }
    
    if (key == GLFW_KEY_Z && (mods & GLFW_MOD_CONTROL) && action == GLFW_PRESS) {
        undo_last_action();
    }
    
    if (key == GLFW_KEY_Y && (mods & GLFW_MOD_CONTROL) && action == GLFW_PRESS) {
        redo_last_action();
    }
    
    if (key == GLFW_KEY_X && (mods & GLFW_MOD_CONTROL) && action == GLFW_PRESS) {
        cut_object();
    }
    
    if (key == GLFW_KEY_C && (mods & GLFW_MOD_CONTROL) && action == GLFW_PRESS) {
        copy_object();
    }
    
    if (key == GLFW_KEY_V && (mods & GLFW_MOD_CONTROL) && action == GLFW_PRESS) {
        paste_object();
    }
    
    if (key == GLFW_KEY_DELETE && action == GLFW_PRESS) {
        if (selected_object) {
            int index = selected_object - objectManager.objects;
            removeObjectWithAction(index);
            selected_object = NULL;
        }
    }
    
    // Show/hide StellAI window with F7
    if (key == GLFW_KEY_F7 && action == GLFW_PRESS) {
        if (is_stellai_initialized()) {
            toggle_stellai_window();
        }
    }
}

// Display loading screen with ImGui
void run_loading_screen(GLFWwindow* window) {
    const char* stages[] = {
        "Initializing...",
        "Loading Textures...",
        "Loading PBR Textures...",
        "Setting Up Skybox...",
        "Setting Up Lighting...",
        "Finalizing..."
    };
    int num_stages = sizeof(stages) / sizeof(stages[0]);
    float progress = 0.0f;
    float progressIncrement = 1.0f / (float)num_stages;

    imgui_init(window);
    
    for (int i = 0; i < num_stages; ++i) {
        printf("Stage: %s, Progress: %.2f%%\n", stages[i], progress * 100);
        loadResources(i, &progress);
        progress += progressIncrement;
        
        // Render loading screen with ImGui
        imgui_new_frame();
        
        imgui_begin_window("Loading", NULL, 0);
        imgui_text(stages[i]);
        // TODO: Add progress bar
        imgui_end_window();
        
        imgui_render();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    imgui_shutdown();
}

// Object selection function
void select_object(int index) {
    if (index >= 0 && index < objectManager.count) {
        selected_object = &objectManager.objects[index];
        printf("Selected object: ID=%d, Index=%d\n", selected_object->id, index);
    }
}

// Toggle object property function
void toggle_object_property(SceneObject* obj, const char* property) {
    if (obj) {
        if (strcmp(property, "usePBR") == 0) {
            obj->object.usePBR = !obj->object.usePBR;
        }
        else if (strcmp(property, "useTexture") == 0) {
            obj->object.useTexture = !obj->object.useTexture;
        }
        else if (strcmp(property, "useColor") == 0) {
            obj->object.useColor = !obj->object.useColor;
        }
        else if (strcmp(property, "useLighting") == 0) {
            obj->object.useLighting = !obj->object.useLighting;
        }
        updateObjectInManager(obj);
    }
}

// Reset GUI function
void reset_gui() {
    show_about = false;
    show_debug = false;
    show_hierarchy = true;
    show_inspector = true;
    show_color_picker = false;
    show_history = false;
    show_settings = false;
    show_controls = false;
    show_change_background = false;
    show_object_creator = false;
    show_stellai = false;
    selected_object = NULL; 
}

// Render the hierarchy window with ImGui
void render_hierarchy_window() {
    if (imgui_begin_window("Hierarchy", &show_hierarchy, 0)) {
        for (int i = 0; i < objectManager.count; i++) {
            SceneObject* sceneObj = &objectManager.objects[i];
            const char* typeName = objectTypeName(sceneObj->object.type);
            
            char label[128];
            snprintf(label, sizeof(label), "%d. %s", i + 1, typeName);
            
            bool is_selected = (selected_object == sceneObj);
            if (is_selected) {
                // Highlight selected object
                // TODO: Use ImGui styling for this
            }
            
            if (imgui_button(label, 0, 0)) {
                select_object(i);
            }
            
            // If selected, show object properties
            if (selected_object == sceneObj) {
                char buffer[128];
                
                snprintf(buffer, sizeof(buffer), "Type: %s", typeName);
                imgui_text(buffer);
                
                snprintf(buffer, sizeof(buffer), "Position: (%.2f, %.2f, %.2f)", 
                        sceneObj->position.x, sceneObj->position.y, sceneObj->position.z);
                imgui_text(buffer);
                
                snprintf(buffer, sizeof(buffer), "Rotation: (%.2f, %.2f, %.2f)", 
                        sceneObj->rotation.x, sceneObj->rotation.y, sceneObj->rotation.z);
                imgui_text(buffer);
                
                snprintf(buffer, sizeof(buffer), "Scale: (%.2f, %.2f, %.2f)", 
                        sceneObj->scale.x, sceneObj->scale.y, sceneObj->scale.z);
                imgui_text(buffer);
                
                snprintf(buffer, sizeof(buffer), "Use Color: %s", sceneObj->object.useColor ? "Yes" : "No");
                imgui_text(buffer);
                
                snprintf(buffer, sizeof(buffer), "Use Texture: %s", sceneObj->object.useTexture ? "Yes" : "No");
                imgui_text(buffer);
                
                snprintf(buffer, sizeof(buffer), "Use PBR: %s", sceneObj->object.usePBR ? "Yes" : "No");
                imgui_text(buffer);
                
                if (imgui_button("Delete Object", 0, 0)) {
                    removeObject(i);
                    selected_object = NULL;
                }
            }
        }
        imgui_end_window();
    }
}

// Render the inspector window with ImGui
void render_inspector_window() {
    if (selected_object != NULL && imgui_begin_window("Inspector", &show_inspector, 0)) {
        imgui_text("Position");
        // TODO: Add sliders for position
        
        imgui_text("Rotation");
        // TODO: Add sliders for rotation
        
        imgui_text("Scale");
        // TODO: Add sliders for scale
        
        imgui_text("Color");
        float color[3] = {
            selected_object->color.x,
            selected_object->color.y,
            selected_object->color.z
        };
        if (imgui_color_picker3("Color", color)) {
            selected_object->color.x = color[0];
            selected_object->color.y = color[1];
            selected_object->color.z = color[2];
        }
        
        imgui_end_window();
    }
}

// Main GUI function using ImGui
void main_gui() {
    // Start new ImGui frame
    imgui_new_frame();
    
    // Main menu bar
    if (imgui_begin_main_menu_bar()) {
        if (imgui_begin_menu("File", true)) {
            if (imgui_menu_item("New Project", NULL, false, true)) {
                new_project();
            }
            if (imgui_menu_item("Open Project...", NULL, false, true)) {
                load_project();
            }
            if (imgui_menu_item("Save Project", NULL, false, true)) {
                save_project();
            }
            if (imgui_menu_item("Save Project As...", NULL, false, true)) {
                save_project(); // TODO: Implement save as
            }
            imgui_separator();
            if (imgui_menu_item("Exit", NULL, false, true)) {
                glfwSetWindowShouldClose(window, GLFW_TRUE);
            }
            imgui_end_menu();
        }
        
        if (imgui_begin_menu("Edit", true)) {
            if (imgui_menu_item("Undo", "Ctrl+Z", false, true)) {
                undo_last_action();
            }
            if (imgui_menu_item("Redo", "Ctrl+Y", false, true)) {
                redo_last_action();
            }
            imgui_separator();
            if (imgui_menu_item("Cut", "Ctrl+X", false, selected_object != NULL)) {
                cut_object();
            }
            if (imgui_menu_item("Copy", "Ctrl+C", false, selected_object != NULL)) {
                copy_object();
            }
            if (imgui_menu_item("Paste", "Ctrl+V", false, clipboard_object != NULL)) {
                paste_object();
            }
            imgui_separator();
            if (imgui_menu_item("Import Model...", NULL, false, true)) {
                import_model();
            }
            imgui_end_menu();
        }
        
        if (imgui_begin_menu("View", true)) {
            if (imgui_menu_item("Hierarchy", NULL, show_hierarchy, true)) {
                show_hierarchy = !show_hierarchy;
            }
            if (imgui_menu_item("Inspector", NULL, show_inspector, true)) {
                show_inspector = !show_inspector;
            }
            if (imgui_menu_item("Debug Info", NULL, show_debug, true)) {
                show_debug = !show_debug;
            }
            imgui_separator();
            if (imgui_menu_item("Dark Theme", NULL, theme_dark, true)) {
                set_theme(true);
            }
            if (imgui_menu_item("Light Theme", NULL, !theme_dark, true)) {
                set_theme(false);
            }
            imgui_end_menu();
        }
        
        if (imgui_begin_menu("Objects", true)) {
            if (imgui_menu_item("Add Cube", NULL, false, true)) {
                addObject(&camera, OBJ_CUBE, texturesEnabled, textureIndex, 
                        colorsEnabled, NULL, *getMaterial("peacockOre"), true);
            }
            if (imgui_menu_item("Add Sphere", NULL, false, true)) {
                addObject(&camera, OBJ_SPHERE, texturesEnabled, textureIndex, 
                        colorsEnabled, NULL, *getMaterial("peacockOre"), true);
            }
            if (imgui_menu_item("Add Pyramid", NULL, false, true)) {
                addObject(&camera, OBJ_PYRAMID, texturesEnabled, textureIndex, 
                        colorsEnabled, NULL, *getMaterial("peacockOre"), true);
            }
            if (imgui_menu_item("Add Plane", NULL, false, true)) {
                addObject(&camera, OBJ_PLANE, texturesEnabled, textureIndex, 
                        colorsEnabled, NULL, *getMaterial("peacockOre"), true);
            }
            if (imgui_menu_item("Add Cylinder", NULL, false, true)) {
                addObject(&camera, OBJ_CYLINDER, texturesEnabled, textureIndex, 
                        colorsEnabled, NULL, *getMaterial("peacockOre"), true);
            }
            imgui_separator();
            if (imgui_menu_item("Add Point Light", NULL, false, true)) {
                createLight(camera.Position, camera.Front, vector(1.0f, 1.0f, 1.0f), 1.0f, LIGHT_POINT);
            }
            if (imgui_menu_item("Add Directional Light", NULL, false, true)) {
                createLight(camera.Position, camera.Front, vector(1.0f, 1.0f, 1.0f), 1.0f, LIGHT_DIRECTIONAL);
            }
            if (imgui_menu_item("Add Spotlight", NULL, false, true)) {
                createLight(camera.Position, camera.Front, vector(1.0f, 1.0f, 1.0f), 1.0f, LIGHT_SPOT);
            }
            imgui_end_menu();
        }
        
        // StellAI menu, only show if available
        if (is_stellai_initialized() && imgui_begin_menu("StellAI", true)) {
            if (imgui_menu_item("AI Tools", NULL, show_stellai, true)) {
                toggle_stellai_window();
            }
            imgui_separator();
            if (imgui_menu_item("Enable AI", NULL, is_stellai_enabled(), true)) {
                // TODO: Add toggle for AI features
            }
            imgui_end_menu();
        }
        
        if (imgui_begin_menu("Help", true)) {
            if (imgui_menu_item("Controls", NULL, show_controls, true)) {
                show_controls = !show_controls;
            }
            if (imgui_menu_item("About", NULL, show_about, true)) {
                show_about = !show_about;
            }
            
            // Debug option for ImGui demo
            #ifdef DEBUG
            static bool show_demo = false;
            if (imgui_menu_item("ImGui Demo", NULL, show_demo, true)) {
                show_demo = !show_demo;
            }
            if (show_demo) {
                imgui_show_demo_window(&show_demo);
            }
            #endif
            
            imgui_end_menu();
        }
        
        imgui_end_main_menu_bar();
    }
    
    // Show hierarchy window
    if (show_hierarchy && !isRunning) {
        render_hierarchy_window();
    }
    
    // Show inspector window
    if (show_inspector && selected_object != NULL && !isRunning) {
        render_inspector_window();
    }
    
    // Update and render StellAI if active
    if (is_stellai_initialized()) {
        stellai_update(0.016f); // Assume 60 FPS for now
    }
    
    // Render ImGui
    imgui_render();
    
    // Also render StellAI overlays if needed
    if (is_stellai_initialized() && show_stellai) {
        stellai_render();
    }
}

// Generate new frame
void generate_new_frame() {
    // Nothing needed here, ImGui frame is generated in main_gui
}

// Render ImGui
void render_imgui() {
    // Nothing needed here, ImGui rendering is done in main_gui
}

// Teardown ImGui
void teardown_imgui() {
    // Shutdown StellAI first
    shutdown_stellai();
    
    // Shutdown ImGui
    imgui_shutdown();
}

// StellAI integration functions
void initialize_stellai() {
    stellai_initialize();
}

void shutdown_stellai() {
    stellai_shutdown();
}

bool is_stellai_initialized() {
    return stellai_is_initialized();
}

bool is_stellai_enabled() {
    return stellai_is_enabled();
}

void show_stellai_window(bool show) {
    show_stellai = show;
    stellai_show_main_window(show);
}

void toggle_stellai_window() {
    show_stellai = !show_stellai;
    stellai_toggle_main_window();
}