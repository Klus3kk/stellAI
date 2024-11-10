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

# Add cJSON source file directly to the project
set(CJSON_SOURCES ${CMAKE_SOURCE_DIR}/external/cjson/cJSON.c)

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

# Define the executable first
add_executable(StellAI ${SOURCES})

# Link libraries
target_link_libraries(StellAI
    glfw
    glad
    glew             # Link GLEW
    SOIL2
    assimp           # Linking Assimp for model loading
    tinyfiledialogs  # Linking tinyfiledialogs for file dialogs
)

# Now specify include directories
target_include_directories(StellAI PRIVATE 
    ${CMAKE_BINARY_DIR}/_deps/glad-src/include   # Path to glad headers
    ${GLFW_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIRS}
    ${SOIL2_INCLUDE_DIRS}
    ${ASSIMP_INCLUDE_DIRS}
    ${TINYFILEDIALOGS_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/include                  # Project's include directory
    ${CMAKE_SOURCE_DIR}/external/nuklear         # Nuklear as a header-only library
    ${CMAKE_SOURCE_DIR}/external/cjson           # cJSON include directory
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
