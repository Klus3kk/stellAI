/**
 * @file StellAI.hpp
 * @brief Main header for StellAI integration with ClueEngine
 */

#ifndef STELLAI_HPP
#define STELLAI_HPP

#include <string>
#include <vector>
#include <memory>
#include <random>

// Include the C headers directly instead of forward declaring
extern "C" {
    struct Vector3;
    struct PBRMaterial;
    struct Model;
}

#include "Vectors.h"
#include "materials.h"
#include "ModelLoad.h"

namespace StellAI {

// Version information
struct Version {
    static inline std::string toString() { return "0.1.0"; }
};

// Forward declarations
namespace WorldGen { class TerrainGenerator; }
namespace ModelGen { class ModelGenerator; }
namespace ShaderGen { class ShaderGenerator; }

//==============================
// Engine Core
//==============================

/**
 * @brief Main StellAI engine class - singleton
 */
class Engine {
private:
    bool initialized = false;
    bool aiEnabled = false;
    
    WorldGen::TerrainGenerator* worldGen = nullptr;
    ModelGen::ModelGenerator* modelGen = nullptr;
    ShaderGen::ShaderGenerator* shaderGen = nullptr;
    
    // Private constructor for singleton
    Engine() = default;
    
public:
    // Delete copy/move constructors and assignments
    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;
    Engine(Engine&&) = delete;
    Engine& operator=(Engine&&) = delete;
    
    // Singleton accessor
    static Engine& getInstance() {
        static Engine instance;
        return instance;
    }
    
    // Core functions
    bool initialize(bool enableAI = true);
    void shutdown();
    
    // Accessors
    WorldGen::TerrainGenerator& getWorldGen() { return *worldGen; }
    ModelGen::ModelGenerator& getModelGen() { return *modelGen; }
    ShaderGen::ShaderGenerator& getShaderGen() { return *shaderGen; }
    
    bool isInitialized() const { return initialized; }
    bool isAIEnabled() const { return aiEnabled; }
};

//==============================
// World Generation
//==============================

namespace WorldGen {

/**
 * @brief Parameters for terrain generation
 */
struct TerrainParams {
    float scale = 1.0f;      // Scale of terrain features
    float roughness = 0.5f;  // How rough the terrain is (0.0-1.0)
    float amplitude = 10.0f; // Height amplitude
    int octaves = 4;         // Noise octaves for detail
    unsigned int seed = 0;   // Random seed
    Vector3 position;        // Position in world space
};

/**
 * @brief Terrain generator class
 */
class TerrainGenerator {
public:
    TerrainGenerator();
    ~TerrainGenerator();
    
    /**
     * @brief Generate terrain mesh from parameters
     * @param params Parameters for terrain generation
     * @return Pointer to generated Model (must be freed by caller)
     */
    Model* generateTerrain(const TerrainParams& params);
    
    /**
     * @brief Apply biomes to a terrain using AI
     * @param terrain The terrain Model to modify
     * @param aiModelName Name of the AI model to use for biome generation
     * @return True if successful
     */
    bool applyBiomes(Model* terrain, const std::string& aiModelName);
};

} // namespace WorldGen

//==============================
// Model Generation
//==============================

namespace ModelGen {

/**
 * @brief Parameters for model generation from text
 */
struct ModelGenParams {
    const char* prompt;      // Text description of the model
    float complexity = 0.5f; // Complexity level (0.0-1.0)
    int resolution = 32;     // Resolution/detail level
    bool texturing = true;   // Generate textures
    Vector3 size;            // Size/scale
};

/**
 * @brief Model generator class
 */
class ModelGenerator {
public:
    ModelGenerator();
    ~ModelGenerator();
    
    /**
     * @brief Generate a 3D model from text description
     * @param params Parameters for model generation
     * @return Pointer to generated Model (must be freed by caller)
     */
    Model* generateFromText(const ModelGenParams& params);
    
    /**
     * @brief Generate a PBR material for a model
     * @param model The model to generate material for
     * @param description Text description of desired material
     * @return Generated PBR material
     */
    PBRMaterial generateMaterial(Model* model, const std::string& description);
};

} // namespace ModelGen

//==============================
// Shader Generation
//==============================

namespace ShaderGen {

/**
 * @brief Parameters for shader generation
 */
struct ShaderGenParams {
    const char* effect;                    // Effect description
    bool optimizeForPerformance = false;   // Optimize for performance vs. quality
    std::vector<std::string> features;     // Additional features to include
};

/**
 * @brief Shader generator class
 */
class ShaderGenerator {
public:
    ShaderGenerator();
    ~ShaderGenerator();
    
    /**
     * @brief Generate custom shader from description
     * @param params Shader generation parameters
     * @return Pair of vertex and fragment shaders as strings
     */
    std::pair<std::string, std::string> generateShader(const ShaderGenParams& params);
    
    /**
     * @brief Optimize existing shader
     * @param vertexShader Vertex shader source
     * @param fragmentShader Fragment shader source
     * @return Optimized vertex and fragment shaders
     */
    std::pair<std::string, std::string> optimizeShader(
        const std::string& vertexShader, 
        const std::string& fragmentShader);
};

} // namespace ShaderGen

} // namespace StellAI

#endif // STELLAI_HPP