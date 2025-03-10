/**
 * @file StellAI_integration.h
 * @brief C-style interface for integrating StellAI with ClueEngine
 */

 #ifndef STELLAI_INTEGRATION_H
 #define STELLAI_INTEGRATION_H
 
 #include <stdbool.h>
 #include "materials.h"
 #include "ModelLoad.h"
 
 #ifdef __cplusplus
 extern "C" {
 #endif
 
 /**
  * Struct for terrain generation parameters
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
  * Struct for model generation parameters
  */
 typedef struct {
     const char* prompt;
     float complexity;
     int resolution;
     bool texturing;
     Vector3 size;
 } StellAI_ModelGenParams;
 
 /**
  * Struct for shader generation parameters
  */
 typedef struct {
     const char* effect;
     bool optimizeForPerformance;
     const char** features;
     int numFeatures;
 } StellAI_ShaderGenParams;
 
 /**
  * Initialize StellAI engine and its components
  * @param enableAI True to enable AI features, false for procedural only
  * @return True if initialization succeeded
  */
 bool StellAI_Initialize(bool enableAI);
 
 /**
  * Shutdown StellAI engine and free resources
  */
 void StellAI_Shutdown();
 
 /**
  * Check if StellAI engine is available
  * @return True if available
  */
 bool StellAI_IsAvailable();
 
 /**
  * Check if AI features are enabled
  * @return True if AI features are enabled
  */
 bool StellAI_IsAIEnabled();
 
 /**
  * Get StellAI version string
  * @param buffer Buffer to store version string
  * @param bufferSize Size of the buffer
  * @return True if successful
  */
 bool StellAI_GetVersion(char* buffer, int bufferSize);
 
 /**
  * Generate terrain using StellAI
  * @param params Terrain generation parameters
  * @return Generated model or NULL on failure
  */
 Model* StellAI_GenerateTerrain(const StellAI_TerrainParams* params);
 
 /**
  * Apply biomes to terrain using AI
  * @param terrain Terrain model to apply biomes to
  * @param aiModelName Name of AI model to use
  * @return True if successful
  */
 bool StellAI_ApplyBiomes(Model* terrain, const char* aiModelName);
 
 /**
  * Generate 3D model from text description
  * @param params Model generation parameters
  * @return Generated model or NULL on failure
  */
 Model* StellAI_GenerateModelFromText(const StellAI_ModelGenParams* params);
 
 /**
  * Generate PBR material for model
  * @param model Model to generate material for
  * @param description Text description of the desired material
  * @return Generated PBR material
  */
 PBRMaterial StellAI_GenerateMaterial(Model* model, const char* description);
 
 /**
  * Generate shader based on description
  * @param params Shader generation parameters
  * @param vertexShader Output buffer for vertex shader code
  * @param vertexShaderSize Size of vertex shader buffer
  * @param fragmentShader Output buffer for fragment shader code
  * @param fragmentShaderSize Size of fragment shader buffer
  * @return True if successful
  */
 bool StellAI_GenerateShader(
     const StellAI_ShaderGenParams* params,
     char* vertexShader, int vertexShaderSize,
     char* fragmentShader, int fragmentShaderSize);
 
 /**
  * Optimize existing shader
  * @param vertexShader Input vertex shader code
  * @param fragmentShader Input fragment shader code
  * @param optimizedVertexShader Output buffer for optimized vertex shader
  * @param vertexShaderSize Size of vertex shader output buffer
  * @param optimizedFragmentShader Output buffer for optimized fragment shader
  * @param fragmentShaderSize Size of fragment shader output buffer
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
 
 #endif /* STELLAI_INTEGRATION_H */