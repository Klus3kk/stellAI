/**
 * @file StellAI_integration.h
 * @brief C interface for StellAI integration with ClueEngine
 */

#ifndef STELLAI_INTEGRATION_H
#define STELLAI_INTEGRATION_H

#include <stdbool.h>

// Include necessary C headers
#include "Vectors.h"
#include "materials.h"
#include "ModelLoad.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief TerrainParams struct for C interface
 */
typedef struct {
    float scale;        // Scale of terrain features
    float roughness;    // How rough the terrain is (0.0-1.0)
    float amplitude;    // Height amplitude
    int octaves;        // Noise octaves for detail
    unsigned int seed;  // Random seed
    Vector3 position;   // Position in world space
} StellAI_TerrainParams;

/**
 * @brief ModelGenParams struct for C interface
 */
typedef struct {
    const char* prompt;       // Text description of the model
    float complexity;         // Complexity level (0.0-1.0)
    int resolution;           // Resolution/detail level
    bool texturing;           // Generate textures
    Vector3 size;             // Size/scale
} StellAI_ModelGenParams;

/**
 * @brief ShaderGenParams struct for C interface
 */
typedef struct {
    const char* effect;                     // Effect description
    bool optimizeForPerformance;            // Optimize for performance vs. quality
    const char** features;                  // Array of additional features to include
    int numFeatures;                        // Number of features in the array
} StellAI_ShaderGenParams;

/**
 * @brief Initialize StellAI
 * @param enableAI Whether to enable AI features
 * @return True if initialization succeeded
 */
bool StellAI_Initialize(bool enableAI);

/**
 * @brief Shutdown StellAI
 */
void StellAI_Shutdown();

/**
 * @brief Check if StellAI is available
 * @return True if available
 */
bool StellAI_IsAvailable();

/**
 * @brief Check if AI features are enabled
 * @return True if AI is enabled
 */
bool StellAI_IsAIEnabled();

/**
 * @brief Get StellAI version string
 * @param buffer Buffer to copy version string into
 * @param bufferSize Size of the buffer
 * @return True if successful
 */
bool StellAI_GetVersion(char* buffer, int bufferSize);

/**
 * @brief Generate terrain mesh
 * @param params Parameters for terrain generation
 * @return Pointer to generated Model (must be freed by caller)
 */
Model* StellAI_GenerateTerrain(const StellAI_TerrainParams* params);

/**
 * @brief Apply biomes to a terrain using AI
 * @param terrain The terrain Model to modify
 * @param aiModelName Name of the AI model to use for biome generation
 * @return True if successful
 */
bool StellAI_ApplyBiomes(Model* terrain, const char* aiModelName);

/**
 * @brief Generate a 3D model from text description
 * @param params Parameters for model generation
 * @return Pointer to generated Model (must be freed by caller)
 */
Model* StellAI_GenerateModelFromText(const StellAI_ModelGenParams* params);

/**
 * @brief Generate a PBR material for a model
 * @param model The model to generate material for
 * @param description Text description of desired material
 * @return Generated PBR material
 */
PBRMaterial StellAI_GenerateMaterial(Model* model, const char* description);

/**
 * @brief Generate custom shader from description
 * @param params Shader generation parameters
 * @param vertexShader Buffer to store generated vertex shader
 * @param vertexShaderSize Size of vertex shader buffer
 * @param fragmentShader Buffer to store generated fragment shader
 * @param fragmentShaderSize Size of fragment shader buffer
 * @return True if successful
 */
bool StellAI_GenerateShader(
    const StellAI_ShaderGenParams* params,
    char* vertexShader, int vertexShaderSize,
    char* fragmentShader, int fragmentShaderSize);

/**
 * @brief Optimize existing shader
 * @param vertexShader Vertex shader source
 * @param fragmentShader Fragment shader source
 * @param optimizedVertexShader Buffer to store optimized vertex shader
 * @param vertexShaderSize Size of vertex shader buffer
 * @param optimizedFragmentShader Buffer to store optimized fragment shader
 * @param fragmentShaderSize Size of fragment shader buffer
 * @return True if successful
 */
bool StellAI_OptimizeShader(
    const char* vertexShader,
    const char* fragmentShader,
    char* optimizedVertexShader, int vertexShaderSize,
    char* optimizedFragmentShader, int fragmentShaderSize);

#ifdef __cplusplus
}
#endif

#endif // STELLAI_INTEGRATION_H