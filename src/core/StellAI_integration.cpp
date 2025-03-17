/**
 * @file StellAI_Integration.cpp
 * @brief Implementation of C-style interface for integrating StellAI with ClueEngine
 */

#include "StellAI_integration.h"
#include "StellAI.hpp"
#include <cstring>
#include <string>
#include <vector>

// Include C headers from ClueEngine, but don't wrap in extern "C"
// to avoid conflicts with C++ templates in other libraries
#include "materials.h"
#include "Vectors.h"
#include "ModelLoad.h"
#include "globals.h"

// Helper function to safely copy strings
static bool safeCopyString(const std::string& src, char* dest, int destSize) {
    if (!dest || destSize <= 0) {
        return false;
    }
    
    size_t copyLength = std::min(src.length(), static_cast<size_t>(destSize - 1));
    std::strncpy(dest, src.c_str(), copyLength);
    dest[copyLength] = '\0';
    return true;
}

bool StellAI_Initialize(bool enableAI) {
    return StellAI::Engine::getInstance().initialize(enableAI);
}

void StellAI_Shutdown() {
    StellAI::Engine::getInstance().shutdown();
}

bool StellAI_IsAvailable() {
    // This should check if the engine is properly initialized
    // In a real implementation, we'd check for required libraries, etc.
    return true;
}

bool StellAI_IsAIEnabled() {
    // In a real implementation, this would check if AI features are enabled
    return StellAI::Engine::getInstance().isAIEnabled();
}

bool StellAI_GetVersion(char* buffer, int bufferSize) {
    if (!buffer || bufferSize <= 0) {
        return false;
    }
    
    std::string version = StellAI::Version::toString();
    return safeCopyString(version, buffer, bufferSize);
}

Model* StellAI_GenerateTerrain(const StellAI_TerrainParams* params) {
    if (!params) {
        return nullptr;
    }
    
    // Convert C struct to C++ struct
    StellAI::WorldGen::TerrainParams cpp_params;
    cpp_params.scale = params->scale;
    cpp_params.roughness = params->roughness;
    cpp_params.amplitude = params->amplitude;
    cpp_params.octaves = params->octaves;
    cpp_params.seed = params->seed;
    cpp_params.position = params->position;
    
    // Call the C++ implementation
    return StellAI::Engine::getInstance().getWorldGen().generateTerrain(cpp_params);
}

bool StellAI_ApplyBiomes(Model* terrain, const char* aiModelName) {
    if (!terrain || !aiModelName) {
        return false;
    }
    
    // Call the C++ implementation
    return StellAI::Engine::getInstance().getWorldGen().applyBiomes(terrain, aiModelName);
}

Model* StellAI_GenerateModelFromText(const StellAI_ModelGenParams* params) {
    if (!params || !params->prompt) {
        return nullptr;
    }
    
    // Convert C struct to C++ struct
    StellAI::ModelGen::ModelGenParams cpp_params;
    cpp_params.prompt = params->prompt;
    cpp_params.complexity = params->complexity;
    cpp_params.resolution = params->resolution;
    cpp_params.texturing = params->texturing;
    cpp_params.size = params->size;
    
    // Call the C++ implementation
    return StellAI::Engine::getInstance().getModelGen().generateFromText(cpp_params);
}

PBRMaterial StellAI_GenerateMaterial(Model* model, const char* description) {
    if (!model || !description) {
        // Return a default material
        PBRMaterial defaultMaterial = { 0 };
        if (materialCount > 0) {
            defaultMaterial = materials[0];
        }
        return defaultMaterial;
    }
    
    // Call the C++ implementation
    return StellAI::Engine::getInstance().getModelGen().generateMaterial(model, description);
}

bool StellAI_GenerateShader(
    const StellAI_ShaderGenParams* params,
    char* vertexShader, int vertexShaderSize,
    char* fragmentShader, int fragmentShaderSize)
{
    if (!params || !params->effect || !vertexShader || !fragmentShader || 
        vertexShaderSize <= 0 || fragmentShaderSize <= 0) {
        return false;
    }
    
    // Convert C struct to C++ struct
    StellAI::ShaderGen::ShaderGenParams cpp_params;
    cpp_params.effect = params->effect;
    cpp_params.optimizeForPerformance = params->optimizeForPerformance;
    
    // Convert features array to vector
    if (params->features && params->numFeatures > 0) {
        for (int i = 0; i < params->numFeatures; ++i) {
            if (params->features[i]) {
                cpp_params.features.push_back(params->features[i]);
            }
        }
    }
    
    // Call the C++ implementation
    auto [vertex, fragment] = StellAI::Engine::getInstance().getShaderGen().generateShader(cpp_params);
    
    // Copy results to output buffers
    bool vertexCopySuccess = safeCopyString(vertex, vertexShader, vertexShaderSize);
    bool fragmentCopySuccess = safeCopyString(fragment, fragmentShader, fragmentShaderSize);
    
    return vertexCopySuccess && fragmentCopySuccess;
}

bool StellAI_OptimizeShader(
    const char* vertexShader,
    const char* fragmentShader,
    char* optimizedVertexShader, int vertexShaderSize,
    char* optimizedFragmentShader, int fragmentShaderSize)
{
    if (!vertexShader || !fragmentShader || !optimizedVertexShader || !optimizedFragmentShader ||
        vertexShaderSize <= 0 || fragmentShaderSize <= 0) {
        return false;
    }
    
    // Call the C++ implementation
    auto [vertex, fragment] = StellAI::Engine::getInstance().getShaderGen().optimizeShader(
        vertexShader, fragmentShader);
    
    // Copy results to output buffers
    bool vertexCopySuccess = safeCopyString(vertex, optimizedVertexShader, vertexShaderSize);
    bool fragmentCopySuccess = safeCopyString(fragment, optimizedFragmentShader, fragmentShaderSize);
    
    return vertexCopySuccess && fragmentCopySuccess;
}