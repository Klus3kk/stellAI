# CMakeLists.txt in the main StellAI directory

cmake_minimum_required(VERSION 3.15)
project(StellAI_3D_Engine)

# Set C++ standard for future upgrades
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable FetchContent for downloading external dependencies
include(FetchContent)

# Fetch GLFW
FetchContent_Declare(
    GLFW
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG latest
)
FetchContent_MakeAvailable(GLFW)

# Fetch GLAD
FetchContent_Declare(
    GLAD
    GIT_REPOSITORY https://github.com/Dav1dde/glad.git
    GIT_TAG main
)
FetchContent_MakeAvailable(GLAD)

# Fetch SOIL2 for image loading
FetchContent_Declare(
    SOIL2
    GIT_REPOSITORY https://github.com/SpartanJ/SOIL2.git
    GIT_TAG master
)
FetchContent_MakeAvailable(SOIL2)

# Fetch cJSON for JSON parsing
FetchContent_Declare(
    cJSON
    GIT_REPOSITORY https://github.com/DaveGamble/cJSON.git
    GIT_TAG master
)
FetchContent_MakeAvailable(cJSON)

# Add include directories for project headers
include_directories(
    include
)

# Add all source files from each folder using GLOB_RECURSE
file(GLOB_RECURSE SOURCES
    "src/core/*.c"
    "src/graphics/*.c"
    "src/gui/*.c"
    "src/resources/*.c"
    "src/entities/*.c"
    "src/utils/*.c"
)

# Create the executable
add_executable(StellAI ${SOURCES})

# Link libraries
target_link_libraries(StellAI
    glfw
    glad
    SOIL2
    cjson
)

# Platform-specific settings (e.g., Windows, Linux, macOS)
if (WIN32)
    target_link_libraries(StellAI opengl32)  # Link OpenGL on Windows
elseif (APPLE)
    find_library(COCOA_LIBRARY Cocoa REQUIRED)
    find_library(IOKIT_LIBRARY IOKit REQUIRED)
    find_library(COREVIDEO_LIBRARY CoreVideo REQUIRED)
    target_link_libraries(StellAI ${COCOA_LIBRARY} ${IOKIT_LIBRARY} ${COREVIDEO_LIBRARY})
elseif (UNIX)
    target_link_libraries(StellAI GL X11 pthread Xrandr Xi dl)  # Link necessary libraries on Linux
endif()

# Additional configuration to ensure GLFW, GLAD, SOIL2, and cJSON are properly linked
target_include_directories(StellAI PRIVATE ${GLFW_INCLUDE_DIRS} ${GLAD_INCLUDE_DIRS} ${SOIL2_INCLUDE_DIRS} ${CJSON_INCLUDE_DIRS})
