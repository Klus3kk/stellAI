/**
 * @file StellAI.hpp
 * @brief C++ API for the StellAI engine
 */

#ifndef STELLAI_HPP
#define STELLAI_HPP

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <tuple>

// Forward declarations for ClueEngine types
struct Model;
struct PBRMaterial;
struct Vector3;

namespace StellAI {

/**
 * StellAI version information
 */
class Version {
public:
    static constexpr int MAJOR = 0;
    static constexpr int MINOR = 1;
    static constexpr int PATCH = 0;
    
    static std::string toString() {
        return std::to_string(MAJOR) + "." + 
                std::to_string(MINOR) + "." + 
                std::to_string(PATCH);
    }
};

/**
 * World generation module
 */
namespace WorldGen {

/**
 * Parameters for terrain generation
 */
struct TerrainParams {
    float scale;
    float roughness;
    float amplitude;
    int octaves;
    int seed;
    Vector3 position;
};

/**
 * Terrain generator component
 */
class TerrainGenerator {
public:
    TerrainGenerator();
    ~TerrainGenerator();
    
    /**
     * Generate terrain using the specified parameters
     * @param params Generation parameters
     * @return Generated terrain model
     */
    Model* generateTerrain(const TerrainParams& params);
    
    /**
     * Apply biomes to an existing terrain using AI
     * @param terrain The terrain model to modify
     * @param aiModelName Name of the AI model to use
     * @return True if successful
     */
    bool applyBiomes(Model* terrain, const std::string& aiModelName);
};

} // namespace WorldGen

/**
 * Model generation module
 */
namespace ModelGen {

/**
 * Parameters for model generation
 */
struct ModelGenParams {
    std::string prompt;
    float complexity;
    int resolution;
    bool texturing;
    Vector3 size;
};

/**
 * Model generator component
 */
class ModelGenerator {
public:
    ModelGenerator();
    ~ModelGenerator();
    
    /**
     * Generate a 3D model from a text description
     * @param params Generation parameters
     * @return Generated model
     */
    Model* generateFromText(const ModelGenParams& params);
    
    /**
     * Generate a PBR material for a model
     * @param model The model to generate material for
     * @param description Text description of the desired material
     * @return Generated material
     */
    PBRMaterial generateMaterial(Model* model, const std::string& description);
};

} // namespace ModelGen

/**
 * Shader generation module
 */
namespace ShaderGen {

/**
 * Parameters for shader generation
 */
struct ShaderGenParams {
    std::string effect;
    bool optimizeForPerformance;
    std::vector<std::string> features;
};

/**
 * Shader generator component
 */
class ShaderGenerator {
public:
    ShaderGenerator();
    ~ShaderGenerator();
    
    /**
     * Generate vertex and fragment shaders from a description
     * @param params Generation parameters
     * @return Pair of vertex and fragment shader code
     */
    std::tuple<std::string, std::string> generateShader(const ShaderGenParams& params);
    
    /**
     * Optimize existing shaders
     * @param vertexShader Existing vertex shader code
     * @param fragmentShader Existing fragment shader code
     * @return Pair of optimized vertex and fragment shader code
     */
    std::tuple<std::string, std::string> optimizeShader(
        const std::string& vertexShader, 
        const std::string& fragmentShader);
};

} // namespace ShaderGen

/**
 * Main StellAI engine singleton
 */
class Engine {
private:
    bool initialized;
    bool aiEnabled;
    
    // Components
    WorldGen::TerrainGenerator terrainGenerator;
    ModelGen::ModelGenerator modelGenerator;
    ShaderGen::ShaderGenerator shaderGenerator;
    
    // Private constructor for singleton
    Engine() : initialized(false), aiEnabled(false) {}
    
public:
    // Delete copy constructor and assignment operator
    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;
    
    /**
     * Get the singleton instance
     */
    static Engine& getInstance() {
        static Engine instance;
        return instance;
    }
    
    /**
     * Initialize the engine
     * @param enableAI Whether to enable AI features
     * @return True if initialization succeeded
     */
    bool initialize(bool enableAI);
    
    /**
     * Shutdown the engine and free resources
     */
    void shutdown();
    
    /**
     * Check if the engine is initialized
     */
    bool isInitialized() const { return initialized; }
    
    /**
     * Check if AI features are enabled
     */
    bool isAIEnabled() const { return aiEnabled; }
    
    /**
     * Get the terrain generator component
     */
    WorldGen::TerrainGenerator& getWorldGen() { return terrainGenerator; }
    
    /**
     * Get the model generator component
     */
    ModelGen::ModelGenerator& getModelGen() { return modelGenerator; }
    
    /**
     * Get the shader generator component
     */
    ShaderGen::ShaderGenerator& getShaderGen() { return shaderGenerator; }
};

} // namespace StellAI

#endif /* STELLAI_HPP */