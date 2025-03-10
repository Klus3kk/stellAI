/**
 * @file gui.c
 * @brief Implementation of GUI functions using ImGui
 */
#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <stdbool.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string.h>

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
#include "materials.h"

// ImGui C API declarations (implemented in imgui_bridge.cpp)
extern void imgui_init(GLFWwindow* window);
extern void imgui_shutdown();
extern void imgui_new_frame();
extern void imgui_render();
extern void imgui_set_dark_theme();
extern void imgui_set_light_theme();
extern bool imgui_begin_window(const char* name, bool* p_open, int flags);
extern void imgui_end_window();
extern bool imgui_button(const char* label, float width, float height);
extern bool imgui_begin_main_menu_bar();
extern void imgui_end_main_menu_bar();
extern bool imgui_begin_menu(const char* label, bool enabled);
extern void imgui_end_menu();
extern bool imgui_menu_item(const char* label, const char* shortcut, bool selected, bool enabled);
extern void imgui_text(const char* text);
extern void imgui_input_text(const char* label, char* buf, size_t buf_size);
extern bool imgui_color_picker3(const char* label, float col[3]);
extern bool imgui_slider_float(const char* label, float* v, float v_min, float v_max);
extern bool imgui_checkbox(const char* label, bool* v);
extern void imgui_separator();
extern void imgui_same_line();
extern bool imgui_collapsing_header(const char* label, int flags);
extern bool imgui_tree_node(const char* label);
extern void imgui_tree_pop();
extern void imgui_columns(int count, const char* id, bool border);
extern void imgui_next_column();
extern void imgui_begin_child(const char* str_id, float width, float height, bool border);
extern void imgui_end_child();
extern void imgui_set_item_default_focus();
extern bool imgui_is_item_focused();
extern void imgui_show_demo_window(bool* p_open);

// StellAI C API declarations
extern void stellai_initialize();
extern void stellai_shutdown();
extern bool stellai_is_initialized();
extern bool stellai_is_enabled();
extern void stellai_show_main_window(bool show);
extern void stellai_toggle_main_window();
extern void stellai_update(float deltaTime);
extern void stellai_render();

// Global variables for GUI state
static bool show_material_window = false;
static bool show_texture_window = false;
static SceneObject* material_window_obj = NULL;
static SceneObject* texture_window_obj = NULL;
extern SceneObject* selected_object;

// Clipboard for copy/paste operations
static SceneObject* clipboard_object = NULL;
static bool isCutOperation = false;

// GUI state flags
GLFWwindow* window = NULL;
bool theme_dark = true;
bool show_about = false;
bool show_debug = false;
bool show_hierarchy = true;
bool show_inspector = true; 
bool show_history = false;
bool show_settings = false; 
bool show_controls = false;
bool show_object_creator = false;
bool show_color_picker = false;
bool show_change_background = false;
bool show_change_texture = false;
bool show_change_material = false;
bool show_stellai = false;

bool is_fullscreen = false;
int windowed_x = 100, windowed_y = 100, windowed_width = 1920, windowed_height = 1080;

// Helper function to return object type name
const char* objectTypeName(ObjectType type) {
    switch (type) {
        case OBJ_CUBE: return "Cube";
        case OBJ_SPHERE: return "Sphere";
        case OBJ_PYRAMID: return "Pyramid";
        case OBJ_CYLINDER: return "Cylinder";
        case OBJ_PLANE: return "Plane";
        case OBJ_MODEL: return "Model";
        default: return "Unknown";
    }
}

// Find the index of the selected object in the object manager
int find_selected_object_index(SceneObject* selected_object) {
    if (selected_object == NULL) return -1;
    
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
    
    show_texture_window = true;
    show_change_texture = true;
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
        PBRMaterial defaultMaterial = *getMaterial("peacockOre");
        addObjectWithAction(OBJ_MODEL, false, -1, true, model, defaultMaterial, false);
    }
}

// Cut, copy, paste functions
void cut_object() {
    if (selected_object) {
        int index = find_selected_object_index(selected_object);
        if (index != -1) {
            // Free previous clipboard if exists
            if (clipboard_object) {
                free(clipboard_object);
            }
            
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
        // Free previous clipboard if exists
        if (clipboard_object) {
            free(clipboard_object);
        }
        
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
            printf("Copied object\n");
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
        printf("Pasted object\n");
    }
}

// Close property windows
void close_object_windows() {
    show_material_window = false;
    show_texture_window = false;
    show_color_picker = false;
    show_change_texture = false;
    show_change_material = false;
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
    if (is_fullscreen) {
        glfwSetWindowMonitor(window, NULL, windowed_x, windowed_y, windowed_width, windowed_height, 0);
        is_fullscreen = false;
    }
    else {
        glfwGetWindowPos(window, &windowed_x, &windowed_y);
        glfwGetWindowSize(window, &windowed_width, &windowed_height);

        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
        is_fullscreen = true;
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
    // Skip input handling if we're editing text in ImGui
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureKeyboard)
        return;
    
    if (key == GLFW_KEY_F && action == GLFW_PRESS && (mods & GLFW_MOD_ALT)) {
        toggle_fullscreen(window);
    }

    if (key == GLFW_KEY_E && action == GLFW_PRESS && (mods & GLFW_MOD_ALT)) {
        glfwSetWindowShouldClose(window, GL_TRUE);  
    }

    if (key == GLFW_KEY_P && action == GLFW_PRESS) {
        isRunning = !isRunning;
        printf("Engine is now %s\n", isRunning ? "running" : "paused");
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
            int index = find_selected_object_index(selected_object);
            if (index != -1) {
                removeObjectWithAction(index);
                selected_object = NULL;
            }
        }
    }
    
    // Show/hide StellAI window with F7
    if (key == GLFW_KEY_F7 && action == GLFW_PRESS) {
        if (is_stellai_initialized()) {
            toggle_stellai_window();
        }
    }
    
    // Camera controls and object creation
    
    // Object creation shortcuts (when engine is not running)
    if (!isRunning) {
        if (key == GLFW_KEY_O && action == GLFW_PRESS) {
            PBRMaterial material = *getMaterial("peacockOre");
            addObjectWithAction(OBJ_PLANE, texturesEnabled, textureIndex, colorsEnabled, NULL, material, usePBR);
        }
        
        if (key == GLFW_KEY_C && action == GLFW_PRESS && !(mods & GLFW_MOD_CONTROL)) {
            PBRMaterial material = *getMaterial("peacockOre");
            addObjectWithAction(OBJ_CUBE, texturesEnabled, textureIndex, colorsEnabled, NULL, material, usePBR);
        }
        
        if (key == GLFW_KEY_H && action == GLFW_PRESS) {
            PBRMaterial material = *getMaterial("peacockOre");
            addObjectWithAction(OBJ_PYRAMID, texturesEnabled, textureIndex, colorsEnabled, NULL, material, usePBR);
        }
        
        if (key == GLFW_KEY_K && action == GLFW_PRESS) {
            PBRMaterial material = *getMaterial("peacockOre");
            addObjectWithAction(OBJ_SPHERE, texturesEnabled, textureIndex, colorsEnabled, NULL, material, usePBR);
        }
        
        if (key == GLFW_KEY_B && action == GLFW_PRESS) {
            PBRMaterial material = *getMaterial("peacockOre");
            addObjectWithAction(OBJ_CYLINDER, texturesEnabled, textureIndex, colorsEnabled, NULL, material, usePBR);
        }
        
        if (key == GLFW_KEY_I && action == GLFW_PRESS) {
            import_model();
        }
        
        // Light creation shortcuts
        if (key == GLFW_KEY_P && action == GLFW_PRESS && (mods & GLFW_MOD_CONTROL)) {
            createLight(camera.Position, camera.Front, vector(1.0f, 1.0f, 1.0f), 1.0f, LIGHT_POINT);
        }
        
        if (key == GLFW_KEY_D && action == GLFW_PRESS && (mods & GLFW_MOD_CONTROL)) {
            createLight(camera.Position, camera.Front, vector(1.0f, 1.0f, 1.0f), 1.0f, LIGHT_DIRECTIONAL);
        }
        
        if (key == GLFW_KEY_S && action == GLFW_PRESS && (mods & GLFW_MOD_CONTROL)) {
            createLight(camera.Position, camera.Front, vector(1.0f, 1.0f, 1.0f), 1.0f, LIGHT_SPOT);
        }
    }
}

// Display loading screen with ImGui
void run_loading_screen(GLFWwindow* window) {
    // Setup temporary ImGui context for loading screen
    imgui_init(window);
    
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

    for (int i = 0; i < num_stages; ++i) {
        printf("Stage: %s, Progress: %.2f%%\n", stages[i], progress * 100);
        
        // Clear the screen
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Load resources for this stage
        loadResources(i, &progress);
        progress += progressIncrement;
        
        // Render loading screen with ImGui
        imgui_new_frame();
        
        // Center the loading window
        bool open = true;
        imgui_begin_window("Loading ClueEngine", &open, 0);
        
        imgui_text(stages[i]);
        
        char progress_text[32];
        snprintf(progress_text, sizeof(progress_text), "Progress: %.0f%%", progress * 100);
        imgui_text(progress_text);
        
        // Simple progress bar
        for (int j = 0; j < 50; j++) {
            if (j < (int)(progress * 50)) {
                imgui_text("#");
                imgui_same_line();
            } else {
                imgui_text("-");
                imgui_same_line();
            }
        }
        
        imgui_end_window();
        
        imgui_render();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    // Clean up temporary ImGui context
    imgui_shutdown();
    
    // Wait a moment before proceeding
    glfwWaitEvents();
    glfwPollEvents();
}

// Object selection function
void select_object(int index) {
    if (index >= 0 && index < objectManager.count) {
        selected_object = &objectManager.objects[index];
        printf("Selected object: ID=%d, Index=%d\n", selected_object->id, index);
        
        // Show inspector automatically when selecting an object
        show_inspector = true;
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
    show_inspector = false;
    show_color_picker = false;
    show_history = false;
    show_settings = false;
    show_controls = false;
    show_change_background = false;
    show_change_texture = false;
    show_change_material = false;
    show_object_creator = false;
    show_stellai = false;
    selected_object = NULL; 
}

// Render hierarchy window
void render_hierarchy_window() {
    bool open = true;
    if (imgui_begin_window("Scene Hierarchy", &show_hierarchy, 0)) {
        if (objectManager.count == 0) {
            imgui_text("No objects in the scene.");
            imgui_text("Use the Objects menu or keyboard shortcuts to add objects.");
        } else {
            imgui_text("Scene Objects:");
            imgui_separator();
            
            for (int i = 0; i < objectManager.count; i++) {
                SceneObject* obj = &objectManager.objects[i];
                
                // Create a unique label for each object
                char label[128];
                snprintf(label, sizeof(label), "%s ##%d", objectTypeName(obj->object.type), i);
                
                bool is_selected = (selected_object == obj);
                
                if (is_selected) {
                    // Highlight selected object
                    imgui_text("> ");
                    imgui_same_line();
                }
                
                if (imgui_button(label, 0, 0)) {
                    select_object(i);
                }
                
                // Right-click context menu
                if (imgui_is_item_focused() && obj == selected_object) {
                    // This would be a popup menu in real ImGui, simplified here
                    imgui_text("  Actions:");
                    
                    char option[64];
                    
                    snprintf(option, sizeof(option), "  Delete ##del%d", i);
                    if (imgui_button(option, 0, 0)) {
                        removeObjectWithAction(i);
                        selected_object = NULL;
                    }
                    
                    snprintf(option, sizeof(option), "  Duplicate ##dup%d", i);
                    if (imgui_button(option, 0, 0)) {
                        copy_object();
                        paste_object();
                    }
                }
            }
        }
        
        imgui_separator();
        
        // Quick add buttons
        imgui_text("Add Object:");
        
        if (imgui_button("Cube", 60, 25)) {
            PBRMaterial material = *getMaterial("peacockOre");
            addObjectWithAction(OBJ_CUBE, texturesEnabled, textureIndex, colorsEnabled, NULL, material, usePBR);
        }
        
        imgui_same_line();
        
        if (imgui_button("Sphere", 60, 25)) {
            PBRMaterial material = *getMaterial("peacockOre");
            addObjectWithAction(OBJ_SPHERE, texturesEnabled, textureIndex, colorsEnabled, NULL, material, usePBR);
        }
        
        imgui_same_line();
        
        if (imgui_button("Plane", 60, 25)) {
            PBRMaterial material = *getMaterial("peacockOre");
            addObjectWithAction(OBJ_PLANE, texturesEnabled, textureIndex, colorsEnabled, NULL, material, usePBR);
        }
        
        if (imgui_button("Pyramid", 60, 25)) {
            PBRMaterial material = *getMaterial("peacockOre");
            addObjectWithAction(OBJ_PYRAMID, texturesEnabled, textureIndex, colorsEnabled, NULL, material, usePBR);
        }
        
        imgui_same_line();
        
        if (imgui_button("Cylinder", 60, 25)) {
            PBRMaterial material = *getMaterial("peacockOre");
            addObjectWithAction(OBJ_CYLINDER, texturesEnabled, textureIndex, colorsEnabled, NULL, material, usePBR);
        }
        
        imgui_same_line();
        
        if (imgui_button("Model...", 60, 25)) {
            import_model();
        }
        
        imgui_end_window();
    }
}

// Render inspector window
void render_inspector_window() {
    if (selected_object != NULL) {
        bool open = true;
        if (imgui_begin_window("Inspector", &show_inspector, 0)) {
            // Object type and ID
            char header[128];
            snprintf(header, sizeof(header), "%s (ID: %d)", 
                     objectTypeName(selected_object->object.type), 
                     selected_object->id);
            imgui_text(header);
            imgui_separator();
            
            // Transform section
            if (imgui_collapsing_header("Transform", 1)) {
                // Position
                float position[3] = {
                    selected_object->position.x,
                    selected_object->position.y,
                    selected_object->position.z
                };
                
                imgui_text("Position");
                
                if (imgui_slider_float("X", &position[0], -50.0f, 50.0f)) {
                    selected_object->position.x = position[0];
                    updateObjectInManager(selected_object);
                }
                
                if (imgui_slider_float("Y", &position[1], -50.0f, 50.0f)) {
                    selected_object->position.y = position[1];
                    updateObjectInManager(selected_object);
                }
                
                if (imgui_slider_float("Z", &position[2], -50.0f, 50.0f)) {
                    selected_object->position.z = position[2];
                    updateObjectInManager(selected_object);
                }
                
                // Rotation
                float rotation[3] = {
                    selected_object->rotation.x,
                    selected_object->rotation.y,
                    selected_object->rotation.z
                };
                
                imgui_text("Rotation");
                
                if (imgui_slider_float("Pitch", &rotation[0], 0.0f, 360.0f)) {
                    selected_object->rotation.x = rotation[0];
                    updateObjectInManager(selected_object);
                }
                
                if (imgui_slider_float("Yaw", &rotation[1], 0.0f, 360.0f)) {
                    selected_object->rotation.y = rotation[1];
                    updateObjectInManager(selected_object);
                }
                
                if (imgui_slider_float("Roll", &rotation[2], 0.0f, 360.0f)) {
                    selected_object->rotation.z = rotation[2];
                    updateObjectInManager(selected_object);
                }
                
                // Scale
                float scale[3] = {
                    selected_object->scale.x,
                    selected_object->scale.y,
                    selected_object->scale.z
                };
                
                imgui_text("Scale");
                
                if (imgui_slider_float("X##scale", &scale[0], 0.1f, 10.0f)) {
                    selected_object->scale.x = scale[0];
                    updateObjectInManager(selected_object);
                }
                
                if (imgui_slider_float("Y##scale", &scale[1], 0.1f, 10.0f)) {
                    selected_object->scale.y = scale[1];
                    updateObjectInManager(selected_object);
                }
                
                if (imgui_slider_float("Z##scale", &scale[2], 0.1f, 10.0f)) {
                    selected_object->scale.z = scale[2];
                    updateObjectInManager(selected_object);
                }
            }
            
            // Material section
            if (imgui_collapsing_header("Material", 1)) {
                // Color
                float color[3] = {
                    selected_object->color.x,
                    selected_object->color.y,
                    selected_object->color.z
                };
                
                if (imgui_color_picker3("Color", color)) {
                    selected_object->color.x = color[0];
                    selected_object->color.y = color[1];
                    selected_object->color.z = color[2];
                    updateObjectInManager(selected_object);
                }
                
                // Alpha/transparency
                float alpha = selected_object->color.w;
                if (imgui_slider_float("Opacity", &alpha, 0.0f, 1.0f)) {
                    selected_object->color.w = alpha;
                    updateObjectInManager(selected_object);
                }
                
                // Texture and PBR toggles
                bool useTexture = selected_object->object.useTexture;
                if (imgui_checkbox("Use Texture", &useTexture)) {
                    toggle_object_property(selected_object, "useTexture");
                }
                
                bool usePBR = selected_object->object.usePBR;
                if (imgui_checkbox("Use PBR", &usePBR)) {
                    toggle_object_property(selected_object, "usePBR");
                }
                
                // Texture and material selection buttons
                if (imgui_button("Change Texture...", 150, 0)) {
                    show_change_texture = true;
                    texture_window_obj = selected_object;
                }
                
                if (usePBR && imgui_button("Change Material...", 150, 0)) {
                    show_change_material = true;
                    material_window_obj = selected_object;
                }
            }
            
            // Additional properties section
            if (imgui_collapsing_header("Properties", 1)) {
                bool useLighting = selected_object->object.useLighting;
                if (imgui_checkbox("Affected by Lighting", &useLighting)) {
                    toggle_object_property(selected_object, "useLighting");
                }
                
                // If it's a model, show model path
                if (selected_object->object.type == OBJ_MODEL) {
                    imgui_text("Model Path:");
                    imgui_text(selected_object->object.data.model.path);
                }
            }
            
            imgui_separator();
            
            // Actions section
            if (imgui_button("Delete Object", 150, 30)) {
                int index = find_selected_object_index(selected_object);
                if (index != -1) {
                    removeObjectWithAction(index);
                    selected_object = NULL;
                }
                imgui_end_window();
                return;
            }
            
            imgui_end_window();
        }
    }
}

// Render texture selector window
void render_texture_selector() {
    if (show_change_texture && texture_window_obj != NULL) {
        bool open = true;
        if (imgui_begin_window("Select Texture", &show_change_texture, 0)) {
            imgui_text("Select a texture to apply:");
            imgui_separator();
            
            // Grid layout for textures
            imgui_columns(3, "texture_grid", false);
            
            for (int i = 0; i < textureCount; i++) {
                if (imgui_button(textureNames[i], 80, 80)) {
                    texture_window_obj->object.textureID = textures[i];
                    texture_window_obj->object.useTexture = true;
                    updateObjectInManager(texture_window_obj);
                    show_change_texture = false;
                }
                imgui_next_column();
            }
            
            // Reset columns
            imgui_columns(1, NULL, false);
            
            imgui_separator();
            if (imgui_button("Cancel", 80, 30)) {
                show_change_texture = false;
            }
            
            imgui_end_window();
        }
    }
}

// Render material selector window
void render_material_selector() {
    if (show_change_material && material_window_obj != NULL) {
        bool open = true;
        if (imgui_begin_window("Select Material", &show_change_material, 0)) {
            imgui_text("Select a PBR material to apply:");
            imgui_separator();
            
            // Grid layout for materials
            imgui_columns(2, "material_grid", false);
            
            for (int i = 0; i < materialCount; i++) {
                if (imgui_button(materialNames[i], 120, 80)) {
                    material_window_obj->object.material = materials[i];
                    material_window_obj->object.usePBR = true;
                    updateObjectInManager(material_window_obj);
                    show_change_material = false;
                }
                imgui_next_column();
            }
            
            // Reset columns
            imgui_columns(1, NULL, false);
            
            imgui_separator();
            if (imgui_button("Cancel", 80, 30)) {
                show_change_material = false;
            }
            
            imgui_end_window();
        }
    }
}

// Render settings window
void render_settings_window() {
    if (show_settings) {
        bool open = true;
        if (imgui_begin_window("Settings", &show_settings, 0)) {
            // Camera settings
            if (imgui_collapsing_header("Camera Settings", 1)) {
                float speed = camera.MovementSpeed;
                if (imgui_slider_float("Movement Speed", &speed, 0.1f, 20.0f)) {
                    camera.MovementSpeed = speed;
                }
                
                float sensitivity = camera.MouseSensitivity;
                if (imgui_slider_float("Mouse Sensitivity", &sensitivity, 0.1f, 5.0f)) {
                    camera.MouseSensitivity = sensitivity;
                }
                
                bool invertY = camera.invertY;
                if (imgui_checkbox("Invert Y Axis", &invertY)) {
                    camera.invertY = invertY;
                }
                
                int camMode = (int)camera.mode;
                char* modes[] = { "FPS Mode", "Free Mode", "Orbit Mode" };
                
                imgui_text("Camera Mode:");
                for (int i = 0; i < 3; i++) {
                    if (imgui_button(modes[i], 100, 25)) {
                        camera.mode = (CameraMode)i;
                    }
                    if (i < 2) imgui_same_line();
                }
            }
            
            // Visual settings
            if (imgui_collapsing_header("Visual Settings", 1)) {
                bool usePBRGlobal = usePBR;
                if (imgui_checkbox("Use PBR Materials", &usePBRGlobal)) {
                    usePBR = usePBRGlobal;
                    toggleOptionWithAction("usePBR", usePBR);
                }
                
                bool texEnabled = texturesEnabled;
                if (imgui_checkbox("Enable Textures", &texEnabled)) {
                    texturesEnabled = texEnabled;
                    toggleOptionWithAction("texturesEnabled", texturesEnabled);
                }
                
                bool lightEnabled = lightingEnabled;
                if (imgui_checkbox("Enable Lighting", &lightEnabled)) {
                    lightingEnabled = lightEnabled;
                    toggleOptionWithAction("lightingEnabled", lightingEnabled);
                }
                
                bool bgEnabled = backgroundEnabled;
                if (imgui_checkbox("Show Skybox", &bgEnabled)) {
                    backgroundEnabled = bgEnabled;
                    toggleOptionWithAction("backgroundEnabled", backgroundEnabled);
                }
                
                if (backgroundEnabled) {
                    if (imgui_button("Change Skybox...", 150, 30)) {
                        show_change_background = true;
                    }
                }
            }
            
            // Interface settings
            if (imgui_collapsing_header("Interface Settings", 1)) {
                bool dark = theme_dark;
                if (imgui_checkbox("Dark Theme", &dark)) {
                    set_theme(dark);
                }
                
                if (imgui_button("Reset Layout", 150, 30)) {
                    reset_gui();
                }
            }
            
            imgui_separator();
            
            if (imgui_button("Close", 80, 30)) {
                show_settings = false;
            }
            
            imgui_end_window();
        }
    }
}

// Render about window
void render_about_window() {
    if (show_about) {
        bool open = true;
        if (imgui_begin_window("About ClueEngine", &show_about, 0)) {
            imgui_text("ClueEngine v1.1.0");
            imgui_text("A 3D engine developed in C with OpenGL");
            imgui_separator();
            
            imgui_text("Created by Łukasz Bielaszewski");
            imgui_text("© 2024 All rights reserved");
            
            imgui_separator();
            
            imgui_text("Engine Features:");
            imgui_text("• OpenGL rendering pipeline");
            imgui_text("• PBR materials and lighting");
            imgui_text("• Multiple geometric primitives");
            imgui_text("• Model loading with Assimp");
            imgui_text("• ImGui-based interface");
            
            if (is_stellai_initialized()) {
                imgui_separator();
                imgui_text("StellAI Extension:");
                imgui_text("• AI-powered procedural generation");
                imgui_text("• Model generation from text");
                imgui_text("• Material generation");
                imgui_text("• Custom shader generation");
            }
            
            imgui_separator();
            
            if (imgui_button("Close", 80, 30)) {
                show_about = false;
            }
            
            imgui_end_window();
        }
    }
}

// Render skybox selector
void render_skybox_selector() {
    if (show_change_background) {
        bool open = true;
        if (imgui_begin_window("Select Skybox", &show_change_background, 0)) {
            imgui_text("Select a skybox:");
            imgui_separator();
            
            // Grid layout for skyboxes
            imgui_columns(2, "skybox_grid", false);
            
            for (int i = 0; i < backgroundCount; i++) {
                char name[32];
                snprintf(name, sizeof(name), "%s ##%d", backgroundNames[i], i);
                
                if (imgui_button(name, 120, 80)) {
                    initSkybox(i + 1); // Skybox indices are 1-based
                    show_change_background = false;
                }
                imgui_next_column();
            }
            
            // Reset columns
            imgui_columns(1, NULL, false);
            
            imgui_separator();
            if (imgui_button("Cancel", 80, 30)) {
                show_change_background = false;
            }
            
            imgui_end_window();
        }
    }
}

// Render debug window
void render_debug_window() {
    if (show_debug) {
        bool open = true;
        if (imgui_begin_window("Debug Information", &show_debug, 0)) {
            imgui_text("Engine Status:");
            imgui_text(isRunning ? "Running" : "Paused");
            
            imgui_separator();
            
            // Performance stats
            char fps_text[32];
            float fps = 1.0f / (float)deltaTime;
            snprintf(fps_text, sizeof(fps_text), "FPS: %.1f", fps);
            imgui_text(fps_text);
            
            // Scene stats
            char objects_text[64];
            snprintf(objects_text, sizeof(objects_text), 
                     "Objects: %d/%d", objectManager.count, MAX_OBJECTS);
            imgui_text(objects_text);
            
            char lights_text[32];
            snprintf(lights_text, sizeof(lights_text), 
                     "Lights: %d/%d", lightCount, MAX_LIGHTS);
            imgui_text(lights_text);
            
            // Camera position
            imgui_text("Camera Position:");
            char cam_pos[64];
            snprintf(cam_pos, sizeof(cam_pos), 
                     "X: %.2f, Y: %.2f, Z: %.2f", 
                     camera.Position.x, camera.Position.y, camera.Position.z);
            imgui_text(cam_pos);
            
            // Camera orientation
            char cam_angles[64];
            snprintf(cam_angles, sizeof(cam_angles), 
                     "Yaw: %.2f, Pitch: %.2f", 
                     camera.Yaw, camera.Pitch);
            imgui_text(cam_angles);
            
            // Selected object info
            imgui_separator();
            
            if (selected_object) {
                imgui_text("Selected Object:");
                
                char obj_type[64];
                snprintf(obj_type, sizeof(obj_type), 
                         "Type: %s, ID: %d", 
                         objectTypeName(selected_object->object.type), 
                         selected_object->id);
                imgui_text(obj_type);
                
                char obj_pos[64];
                snprintf(obj_pos, sizeof(obj_pos), 
                         "Position: X: %.2f, Y: %.2f, Z: %.2f", 
                         selected_object->position.x, 
                         selected_object->position.y, 
                         selected_object->position.z);
                imgui_text(obj_pos);
            } else {
                imgui_text("No object selected");
            }
            
            imgui_separator();
            
            // Action history
            imgui_text("Action History:");
            char history_text[32];
            snprintf(history_text, sizeof(history_text), 
                     "Actions: %d", historyCount);
            imgui_text(history_text);
            
            if (imgui_button("Close", 80, 30)) {
                show_debug = false;
            }
            
            imgui_end_window();
        }
    }
}

// Render controls help window
void render_controls_window() {
    if (show_controls) {
        bool open = true;
        if (imgui_begin_window("Controls", &show_controls, 0)) {
            imgui_text("Keyboard Controls:");
            imgui_separator();
            
            // Navigation controls
            if (imgui_collapsing_header("Camera & Navigation", 1)) {
                imgui_text("W - Move forward");
                imgui_text("A - Move left");
                imgui_text("S - Move backward");
                imgui_text("D - Move right");
                imgui_text("Space - Move up");
                imgui_text("Left Shift - Move down");
                imgui_text("Mouse - Look around");
            }
            
            // Object controls
            if (imgui_collapsing_header("Object Creation", 1)) {
                imgui_text("O - Create Plane");
                imgui_text("C - Create Cube");
                imgui_text("H - Create Pyramid");
                imgui_text("K - Create Sphere");
                imgui_text("B - Create Cylinder");
                imgui_text("I - Import 3D Model");
            }
            
            // Light controls
            if (imgui_collapsing_header("Light Creation", 1)) {
                imgui_text("Ctrl+P - Create Point Light");
                imgui_text("Ctrl+D - Create Directional Light");
                imgui_text("Ctrl+S - Create Spotlight");
            }
            
            // Editor controls
            if (imgui_collapsing_header("Editor Controls", 1)) {
                imgui_text("P - Pause/Resume engine");
                imgui_text("Alt+F - Toggle fullscreen");
                imgui_text("F1 - Toggle UI theme");
                imgui_text("F7 - Toggle StellAI window (if available)");
                imgui_text("Ctrl+Z - Undo last action");
                imgui_text("Ctrl+Y - Redo last action");
                imgui_text("Ctrl+X - Cut selected object");
                imgui_text("Ctrl+C - Copy selected object");
                imgui_text("Ctrl+V - Paste object");
                imgui_text("Delete - Delete selected object");
                imgui_text("Alt+E - Exit application");
            }
            
            imgui_separator();
            
            if (imgui_button("Close", 80, 30)) {
                show_controls = false;
            }
            
            imgui_end_window();
        }
    }
}

// Main GUI function
void main_gui() {
    // Start new ImGui frame
    imgui_new_frame();
    
    // Main menu bar
    if (imgui_begin_main_menu_bar()) {
        if (imgui_begin_menu("File", true)) {
            if (imgui_menu_item("New Project", "Ctrl+N", false, true)) {
                new_project();
            }
            if (imgui_menu_item("Open Project...", "Ctrl+O", false, true)) {
                load_project();
            }
            if (imgui_menu_item("Save Project", "Ctrl+S", false, true)) {
                save_project();
            }
            if (imgui_menu_item("Save Project As...", "Ctrl+Shift+S", false, true)) {
                save_project(); // This should be save_project_as in a full implementation
            }
            imgui_separator();
            if (imgui_menu_item("Exit", "Alt+E", false, true)) {
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
            if (imgui_menu_item("Select All", "Ctrl+A", false, false)) {
                // Not implemented - would select all objects
            }
            if (imgui_menu_item("Deselect", "Esc", false, selected_object != NULL)) {
                selected_object = NULL;
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
            if (imgui_menu_item("Settings", NULL, show_settings, true)) {
                show_settings = !show_settings;
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
            if (imgui_menu_item("Add Cube", "C", false, true)) {
                PBRMaterial material = *getMaterial("peacockOre");
                addObjectWithAction(OBJ_CUBE, texturesEnabled, textureIndex, colorsEnabled, NULL, material, usePBR);
            }
            if (imgui_menu_item("Add Sphere", "K", false, true)) {
                PBRMaterial material = *getMaterial("peacockOre");
                addObjectWithAction(OBJ_SPHERE, texturesEnabled, textureIndex, colorsEnabled, NULL, material, usePBR);
            }
            if (imgui_menu_item("Add Pyramid", "H", false, true)) {
                PBRMaterial material = *getMaterial("peacockOre");
                addObjectWithAction(OBJ_PYRAMID, texturesEnabled, textureIndex, colorsEnabled, NULL, material, usePBR);
            }
            if (imgui_menu_item("Add Plane", "O", false, true)) {
                PBRMaterial material = *getMaterial("peacockOre");
                addObjectWithAction(OBJ_PLANE, texturesEnabled, textureIndex, colorsEnabled, NULL, material, usePBR);
            }
            if (imgui_menu_item("Add Cylinder", "B", false, true)) {
                PBRMaterial material = *getMaterial("peacockOre");
                addObjectWithAction(OBJ_CYLINDER, texturesEnabled, textureIndex, colorsEnabled, NULL, material, usePBR);
            }
            if (imgui_menu_item("Import Model...", "I", false, true)) {
                import_model();
            }
            imgui_separator();
            if (imgui_menu_item("Add Point Light", "Ctrl+P", false, true)) {
                createLight(camera.Position, camera.Front, vector(1.0f, 1.0f, 1.0f), 1.0f, LIGHT_POINT);
            }
            if (imgui_menu_item("Add Directional Light", "Ctrl+D", false, true)) {
                createLight(camera.Position, camera.Front, vector(1.0f, 1.0f, 1.0f), 1.0f, LIGHT_DIRECTIONAL);
            }
            if (imgui_menu_item("Add Spotlight", "Ctrl+S", false, true)) {
                createLight(camera.Position, camera.Front, vector(1.0f, 1.0f, 1.0f), 1.0f, LIGHT_SPOT);
            }
            imgui_end_menu();
        }
        
        // StellAI menu, only show if available
        if (is_stellai_initialized() && imgui_begin_menu("StellAI", true)) {
            if (imgui_menu_item("AI Tools", "F7", show_stellai, true)) {
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
        
        // Engine status indicator (right-aligned)
        if (isRunning) {
            imgui_text("                                      "); // Padding for right-alignment
            imgui_same_line();
            imgui_text("Running");
        } else {
            imgui_text("                                      "); // Padding for right-alignment
            imgui_same_line();
            imgui_text("Paused (P)");
        }
        
        imgui_end_main_menu_bar();
    }
    
    // Show hierarchy window
    if (show_hierarchy) {
        render_hierarchy_window();
    }
    
    // Show inspector window
    if (show_inspector && selected_object != NULL) {
        render_inspector_window();
    }
    
    // Render secondary windows
    render_texture_selector();
    render_material_selector();
    render_settings_window();
    render_about_window();
    render_skybox_selector();
    render_debug_window();
    render_controls_window();
    
    // Update and render StellAI if active
    if (is_stellai_initialized()) {
        stellai_update(0.016f); // Assume ~60 FPS for now
        
        // Render StellAI overlays if shown
        if (show_stellai) {
            stellai_render();
        }
    }
    
    // Render ImGui
    imgui_render();
}

// Generate new frame
void generate_new_frame() {
    // Nothing needed here - ImGui frame is created in main_gui
}

// Render ImGui - duplicated from main_gui but kept for backward compatibility
void render_imgui() {
    // Nothing needed here - ImGui rendering is done in main_gui
}

// Teardown ImGui
void teardown_imgui() {
    // Cleanup clipboard
    if (clipboard_object) {
        free(clipboard_object);
        clipboard_object = NULL;
    }
    
    // Shutdown StellAI first if initialized
    if (is_stellai_initialized()) {
        shutdown_stellai();
    }
    
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