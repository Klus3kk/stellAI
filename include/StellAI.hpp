/**
 * @file StellAI.hpp
 * @brief Core header file for StellAI engine
 * 
 * This header defines the main components of the StellAI engine,
 * which extends the ClueEngine with AI-driven capabilities.
 */

 #ifndef STELLAI_HPP
 #define STELLAI_HPP
 
 #include <string>
 #include <vector>
 #include <memory>
 #include <functional>
 #include <unordered_map>
 #include <mutex>
 
 // Forward declarations for C components from ClueEngine
 extern "C" {
     struct Camera;
     struct SceneObject;
     struct PBRMaterial;
     struct Model;
     struct Vector3;
     struct Vector4;
     struct Matrix4x4;
 }
 
 namespace StellAI {
 
 /**
  * @brief Version information for StellAI
  */
 struct Version {
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
  * @brief Base class for all AI models in StellAI
  */
 class AIModel {
 public:
     virtual ~AIModel() = default;
     
     /**
      * @brief Load an AI model from a file
      * @param path Path to the model file
      * @return true if loaded successfully, false otherwise
      */
     virtual bool loadFromFile(const std::string& path) = 0;
     
     /**
      * @brief Check if the model is loaded and ready
      * @return true if ready, false otherwise
      */
     virtual bool isReady() const = 0;
     
     /**
      * @brief Get the name of the model
      * @return Model name
      */
     virtual std::string getName() const = 0;
     
     /**
      * @brief Get the type of the model
      * @return Model type as string
      */
     virtual std::string getType() const = 0;
 };
 
 /**
  * @brief Manager for AI models
  */
 class AIModelManager {
 public:
     static AIModelManager& getInstance() {
         static AIModelManager instance;
         return instance;
     }
     
     /**
      * @brief Register a new AI model
      * @param name Name to register the model under
      * @param model Shared pointer to the model
      * @return true if registered successfully, false otherwise
      */
     bool registerModel(const std::string& name, std::shared_ptr<AIModel> model) {
         std::lock_guard<std::mutex> lock(modelsMutex);
         if (models.find(name) != models.end()) {
             return false; // Model with this name already exists
         }
         models[name] = model;
         return true;
     }
     
     /**
      * @brief Get a model by name
      * @param name Name of the model to retrieve
      * @return Shared pointer to the model, or nullptr if not found
      */
     std::shared_ptr<AIModel> getModel(const std::string& name) {
         std::lock_guard<std::mutex> lock(modelsMutex);
         auto it = models.find(name);
         if (it != models.end()) {
             return it->second;
         }
         return nullptr;
     }
 
 private:
     AIModelManager() = default;
     ~AIModelManager() = default;
     
     AIModelManager(const AIModelManager&) = delete;
     AIModelManager& operator=(const AIModelManager&) = delete;
     
     std::unordered_map<std::string, std::shared_ptr<AIModel>> models;
     std::mutex modelsMutex;
 };
 
 namespace WorldGen {
 
 /**
  * @brief Parameters for procedural terrain generation
  */
 struct TerrainParams {
     float scale = 1.0f;
     float roughness = 0.5f;
     float amplitude = 10.0f;
     int octaves = 4;
     int seed = 12345;
     Vector3 position = {0.0f, 0.0f, 0.0f};
 };
 
 /**
  * @brief Generator for procedural terrain
  */
 class TerrainGenerator {
 public:
     TerrainGenerator();
     ~TerrainGenerator();
     
     /**
      * @brief Generate a terrain mesh
      * @param params Parameters for terrain generation
      * @return Pointer to the generated Model, or nullptr on failure
      */
     Model* generateTerrain(const TerrainParams& params);
     
     /**
      * @brief Apply biomes to a terrain based on AI analysis
      * @param terrain Terrain to apply biomes to
      * @param aiModelName Name of the AI model to use for biome classification
      * @return true if successful, false otherwise
      */
     bool applyBiomes(Model* terrain, const std::string& aiModelName);
 };
 
 } // namespace WorldGen
 
 namespace ModelGen {
 
 /**
  * @brief Parameters for AI-driven model generation
  */
 struct ModelGenParams {
     std::string prompt;        // Text description of the model to generate
     float complexity = 0.5f;   // How complex the model should be (0.0-1.0)
     int resolution = 32;       // Resolution of generated model
     bool texturing = true;     // Whether to generate textures
     Vector3 size = {1.0f, 1.0f, 1.0f}; // Size of the model
 };
 
 /**
  * @brief Generator for AI-created 3D models
  */
 class ModelGenerator {
 public:
     ModelGenerator();
     ~ModelGenerator();
     
     /**
      * @brief Generate a 3D model from text description
      * @param params Parameters for model generation
      * @return Pointer to the generated Model, or nullptr on failure
      */
     Model* generateFromText(const ModelGenParams& params);
     
     /**
      * @brief Generate PBR materials for a model
      * @param model Model to generate materials for
      * @param description Text description of the desired material
      * @return Generated PBR material
      */
     PBRMaterial generateMaterial(Model* model, const std::string& description);
 };
 
 } // namespace ModelGen
 
 namespace ShaderGen {
 
 /**
  * @brief Parameters for shader generation
  */
 struct ShaderGenParams {
     std::string effect;        // Desired visual effect
     std::vector<std::string> features; // Shader features to include
     bool optimizeForPerformance = false; // Whether to optimize for performance over quality
 };
 
 /**
  * @brief Generator for AI-driven shaders
  */
 class ShaderGenerator {
 public:
     ShaderGenerator();
     ~ShaderGenerator();
     
     /**
      * @brief Generate shader code based on parameters
      * @param params Parameters for shader generation
      * @return Pair of vertex and fragment shader code
      */
     std::pair<std::string, std::string> generateShader(const ShaderGenParams& params);
     
     /**
      * @brief Optimize existing shader code using AI
      * @param vertexShader Vertex shader code to optimize
      * @param fragmentShader Fragment shader code to optimize
      * @return Optimized shader code
      */
     std::pair<std::string, std::string> optimizeShader(
         const std::string& vertexShader, 
         const std::string& fragmentShader
     );
 };
 
 } // namespace ShaderGen
 
 /**
  * @brief Main interface for StellAI engine
  */
 class Engine {
 public:
     static Engine& getInstance() {
         static Engine instance;
         return instance;
     }
     
     /**
      * @brief Initialize the StellAI engine
      * @param enableAI Whether to enable AI features
      * @return true if initialized successfully, false otherwise
      */
     bool initialize(bool enableAI = true);
     
     /**
      * @brief Shut down the StellAI engine
      */
     void shutdown();
     
     /**
      * @brief Get the WorldGen module
      * @return Reference to the WorldGen::TerrainGenerator
      */
     WorldGen::TerrainGenerator& getWorldGen() { return terrainGenerator; }
     
     /**
      * @brief Get the ModelGen module
      * @return Reference to the ModelGen::ModelGenerator
      */
     ModelGen::ModelGenerator& getModelGen() { return modelGenerator; }
     
     /**
      * @brief Get the ShaderGen module
      * @return Reference to the ShaderGen::ShaderGenerator
      */
     ShaderGen::ShaderGenerator& getShaderGen() { return shaderGenerator; }
     
 private:
     Engine() = default;
     ~Engine() = default;
     
     Engine(const Engine&) = delete;
     Engine& operator=(const Engine&) = delete;
     
     bool initialized = false;
     bool aiEnabled = false;
     
     WorldGen::TerrainGenerator terrainGenerator;
     ModelGen::ModelGenerator modelGenerator;
     ShaderGen::ShaderGenerator shaderGenerator;
 };
 
 } // namespace StellAI
 
 #endif // STELLAI_HPP