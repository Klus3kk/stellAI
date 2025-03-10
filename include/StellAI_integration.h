/**
 * @file StellAI_Integration.h
 * @brief C-style interface for integrating StellAI with ClueEngine
 */

 #ifndef STELLAI_INTEGRATION_H
 #define STELLAI_INTEGRATION_H
 
 #ifdef __cplusplus
 extern "C" {
 #endif
 
 #include <stdbool.h>
 #include "ModelLoad.h"
 #include "materials.h"
 #include "Vectors.h"
 
 /**
  * @brief Initialize the StellAI engine
  * @param enableAI Whether to enable AI features
  * @return true if initialized successfully, false otherwise
  */
 bool StellAI_Initialize(bool enableAI);
 
 /**
  * @brief Shutdown the StellAI engine
  */
 void StellAI_Shutdown();
 
 /**
  * @brief Check if StellAI is available and initialized
  * @return true if available, false otherwise
  */
 bool StellAI_IsAvailable();
 
 /**
  * @brief Check if AI features are enabled
  * @return true if enabled, false otherwise
  */
 bool StellAI_IsAIEnabled();
 
 /**
  * @brief Get the version string of StellAI
  * @param buffer Buffer to store the version string
  * @param bufferSize Size of the buffer
  * @return true if successful, false otherwise
  */
 bool StellAI_GetVersion(char* buffer, int bufferSize);
 
 // Terrain generation functions
 
 /**
  * @brief Parameters for terrain generation
  */
 typedef struct {
     float scale;
     float roughness;
     float amplitude;
     int octaves;
     int seed;
     Vector3 position;
 } StellAI_TerrainParams;
 
 /**
  * @brief Generate a terrain model
  * @param params Parameters for terrain generation
  * @return Pointer to the generated Model, or NULL on failure
  */
 Model* StellAI_GenerateTerrain(const StellAI_TerrainParams* params);
 
 /**
  * @brief Apply biomes to a terrain based on AI analysis
  * @param terrain Terrain to apply biomes to
  * @param aiModelName Name of the AI model to use for biome classification
  * @return true if successful, false otherwise
  */
 bool StellAI_ApplyBiomes(Model* terrain, const char* aiModelName);
 
 // Model generation functions
 
 /**
  * @brief Parameters for AI-driven model generation
  */
 typedef struct {
     const char* prompt;
     float complexity;
     int resolution;
     bool texturing;
     Vector3 size;
 } StellAI_ModelGenParams;
 
 /**
  * @brief Generate a 3D model from text description
  * @param params Parameters for model generation
  * @return Pointer to the generated Model, or NULL on failure
  */
 Model* StellAI_GenerateModelFromText(const StellAI_ModelGenParams* params);
 
 /**
  * @brief Generate PBR materials for a model
  * @param model Model to generate materials for
  * @param description Text description of the desired material
  * @return Generated PBR material
  */
 PBRMaterial StellAI_GenerateMaterial(Model* model, const char* description);
 
 // Shader generation functions
 
 /**
  * @brief Parameters for shader generation
  */
 typedef struct {
     const char* effect;
     const char** features;
     int numFeatures;
     bool optimizeForPerformance;
 } StellAI_ShaderGenParams;
 
 /**
  * @brief Generate shader code based on parameters
  * @param params Parameters for shader generation
  * @param vertexShader Buffer to store the vertex shader code
  * @param vertexShaderSize Size of the vertex shader buffer
  * @param fragmentShader Buffer to store the fragment shader code
  * @param fragmentShaderSize Size of the fragment shader buffer
  * @return true if successful, false otherwise
  */
 bool StellAI_GenerateShader(
     const StellAI_ShaderGenParams* params,
     char* vertexShader, int vertexShaderSize,
     char* fragmentShader, int fragmentShaderSize
 );
 
 /**
  * @brief Optimize existing shader code using AI
  * @param vertexShader Vertex shader code to optimize
  * @param fragmentShader Fragment shader code to optimize
  * @param optimizedVertexShader Buffer to store the optimized vertex shader code
  * @param vertexShaderSize Size of the optimized vertex shader buffer
  * @param optimizedFragmentShader Buffer to store the optimized fragment shader code
  * @param fragmentShaderSize Size of the optimized fragment shader buffer
  * @return true if successful, false otherwise
  */
 bool StellAI_OptimizeShader(
     const char* vertexShader,
     const char* fragmentShader,
     char* optimizedVertexShader, int vertexShaderSize,
     char* optimizedFragmentShader, int fragmentShaderSize
 );
 
 #ifdef __cplusplus
 }
 #endif
 
 #endif // STELLAI_INTEGRATION_H