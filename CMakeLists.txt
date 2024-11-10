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
    GIT_TAG v2.0.6  # Use a specific tag instead of 'main'
)
FetchContent_MakeAvailable(GLAD)

# Fetch GLEW
FetchContent_Declare(
    GLEW
    GIT_REPOSITORY https://github.com/nigels-com/glew.git
    GIT_TAG master
)
FetchContent_MakeAvailable(GLEW)

# Fetch SOIL2 for image loading
FetchContent_Declare(
    SOIL2
    GIT_REPOSITORY https://github.com/SpartanJ/SOIL2.git
    GIT_TAG master
)
FetchContent_MakeAvailable(SOIL2)

# Fetch Assimp for model loading
FetchContent_Declare(
    Assimp
    GIT_REPOSITORY https://github.com/assimp/assimp.git
    GIT_TAG master
)
FetchContent_MakeAvailable(Assimp)

# Fetch tinyfiledialogs for file dialogs
FetchContent_Declare(
    tinyfiledialogs
    GIT_REPOSITORY https://git.code.sf.net/p/tinyfiledialogs/code
    GIT_TAG master
)
FetchContent_MakeAvailable(tinyfiledialogs)

# Add Nuklear (assuming it's locally included in the project)
add_library(nuklear STATIC
    nuklear/nuklear.h
    nuklear/nuklear_glfw_gl3.h
)


# Add include directories for project headers
include_directories(
    include
    external/cjson  # Add the cJSON include directory
)

# Add cJSON source file directly to the project
set(CJSON_SOURCES external/cjson/cJSON.c)

# Add all other source files from each folder using GLOB_RECURSE
file(GLOB_RECURSE SOURCES
    "src/core/*.c"
    "src/graphics/*.c"
    "src/gui/*.c"
    "src/resources/*.c"
    "src/entities/*.c"
    "src/utils/*.c"
)

# Include cJSON source file with other sources
list(APPEND SOURCES ${CJSON_SOURCES})

# Create the executable
add_executable(StellAI ${SOURCES})

# Link libraries
target_link_libraries(StellAI
    glfw
    glad
    glew             # Link GLEW
    SOIL2
    assimp           # Linking Assimp for model loading
    nuklear          # Linking Nuklear for GUI
    tinyfiledialogs  # Linking tinyfiledialogs for file dialogs
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

# Additional configuration to ensure all libraries are properly linked
target_include_directories(StellAI PRIVATE 
    ${GLFW_INCLUDE_DIRS} 
    ${GLAD_INCLUDE_DIRS} 
    ${GLEW_INCLUDE_DIRS} 
    ${SOIL2_INCLUDE_DIRS} 
    ${ASSIMP_INCLUDE_DIRS} 
    ${TINYFILEDIALOGS_INCLUDE_DIRS}
)
