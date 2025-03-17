/**
 * @file main.c
 * @brief Main entry point for ClueEngine with ImGui integration and StellAI extension
 */
#include "gui.h"
#include "rendering.h"
#include "globals.h"
#include "file_operations.h"
#include "ObjectManager.h"
#include "resource_loader.h"
#include "materials.h"
#include "textures.h"
#include "lightshading.h"
#include "background.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Forward declarations
static void process_input(GLFWwindow* window, float deltaTime);
static float calculate_delta_time();

// Global time variables
static float last_frame_time = 0.0f;
static float delta_time = 0.0f;

int main(void) {
    // Hide console on Windows (for release builds)
    #ifdef _WIN32
        #ifdef NDEBUG
            #include <windows.h>
            ShowWindow(GetConsoleWindow(), SW_HIDE);
        #endif
    #endif

    // Initialize random seed
    srand((unsigned int)time(NULL));

    // Setup OpenGL and GLFW
    setup();

    // Run loading screen
    initLoadingScreen(screen.window);
    run_loading_screen(screen.window);

    // Set up callbacks
    glfwSetKeyCallback(screen.window, key_callback);
    glfwSetFramebufferSizeCallback(screen.window, framebuffer_size_callback);
    glfwSetWindowSizeCallback(screen.window, resize_callback);

    // Load resources
    printf("Loading textures...\n");
    load_texture();
    
    printf("Loading materials...\n");
    load_material();
    
    printf("Setting up skybox...\n");
    initSkybox(1); // Default to the first skybox
    
    printf("Setting up lighting...\n");
    initLightingSystem();
    
    // Create a default scene
    printf("Creating default scene...\n");
    PBRMaterial defaultMaterial = *getMaterial("peacockOre");
    
    // Add a ground plane
    addObject(&camera, OBJ_PLANE, true, 0, true, NULL, defaultMaterial, true);
    
    // Add some example objects
    Vector3 positions[] = {
        {-3.0f, 1.0f, 0.0f},  // Left
        {0.0f, 1.0f, 0.0f},   // Center
        {3.0f, 1.0f, 0.0f}    // Right
    };
    
    // Add a cube, sphere, and pyramid
    addObject(&camera, OBJ_CUBE, true, 0, true, NULL, defaultMaterial, true);
    objectManager.objects[objectManager.count-1].position = positions[0];
    
    addObject(&camera, OBJ_SPHERE, true, 0, true, NULL, defaultMaterial, true);
    objectManager.objects[objectManager.count-1].position = positions[1];
    
    addObject(&camera, OBJ_PYRAMID, true, 0, true, NULL, defaultMaterial, true);
    objectManager.objects[objectManager.count-1].position = positions[2];
    
    // Add a point light
    createLight((Vector3){0.0f, 5.0f, 0.0f}, (Vector3){0.0f, -1.0f, 0.0f}, (Vector3){1.0f, 1.0f, 1.0f}, 1.5f, LIGHT_POINT);
    
    // Main render loop
    printf("Starting main loop...\n");
    while (!glfwWindowShouldClose(screen.window)) {
        // Calculate delta time
        delta_time = calculate_delta_time();
        
        // Process input
        glfwPollEvents();
        process_input(screen.window, delta_time);
        
        // Update game state if running
        if (isRunning) {
            update(delta_time);
        }
        
        // Render
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        render();
        
        // Render GUI
        main_gui();
        
        // Swap buffers
        glfwSwapBuffers(screen.window);
    }

    // Cleanup
    cleanupObjects();
    teardown_imgui();
    end();
    
    printf("Application terminated normally.\n");
    return 0;
}

/**
 * Process keyboard and mouse input
 */
/**
 * Calculate delta time between frames
 */
static float calculate_delta_time() {
    float current_time = (float)glfwGetTime();
    float delta = current_time - last_frame_time;
    last_frame_time = current_time;
    
    // Cap delta time to prevent large jumps after pauses
    if (delta > 0.1f) {
        delta = 0.1f;
    }
    
    return delta;
}

/**
 * Process keyboard and mouse input
 */
static void process_input(GLFWwindow* window, float deltaTime) {
    // Skip camera controls when paused or in UI mode
    if (!isRunning) {
        return;
    }
    
    // Camera movement
    if (cameraEnabled) {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            processKeyboard(&camera, FORWARD, deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            processKeyboard(&camera, BACKWARD, deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            processKeyboard(&camera, LEFT, deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            processKeyboard(&camera, RIGHT, deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            processKeyboard(&camera, SPACE, deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
            processKeyboard(&camera, SHIFT, deltaTime);
        }
    }

    // Check for escape key to pause engine
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        isRunning = false;
    }

    // Toggle debug overlay with F3
    static bool f3Pressed = false;
    if (glfwGetKey(window, GLFW_KEY_F3) == GLFW_PRESS && !f3Pressed) {
        extern bool show_debug;
        show_debug = !show_debug;
        f3Pressed = true;
    } else if (glfwGetKey(window, GLFW_KEY_F3) == GLFW_RELEASE) {
        f3Pressed = false;
    }

    // Additional shortcuts
    static bool f5Pressed = false;
    if (glfwGetKey(window, GLFW_KEY_F5) == GLFW_PRESS && !f5Pressed) {
        // Quick save
        save_project();
        f5Pressed = true;
    } else if (glfwGetKey(window, GLFW_KEY_F5) == GLFW_RELEASE) {
        f5Pressed = false;
    }
}