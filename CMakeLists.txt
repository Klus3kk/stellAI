cmake_minimum_required(VERSION 3.15)
project(StellAI_3D_Engine)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable FetchContent for dependencies
include(FetchContent)

# Fetch GLFW
FetchContent_Declare(
    GLFW
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG latest
)
FetchContent_MakeAvailable(GLFW)

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

# Include cJSON, glad sources directly
set(CJSON_SOURCES ${CMAKE_SOURCE_DIR}/external/cjson/cJSON.c)
set(GLAD_SOURCES ${CMAKE_SOURCE_DIR}/external/glad/src/glad.c)

# Gather other source files
file(GLOB_RECURSE SOURCES
    "src/core/*.c"
    "src/graphics/*.c"
    "src/gui/*.c"
    "src/resources/*.c"
    "src/entities/*.c"
    "src/utils/*.c"
)

# Include cJSON, GLAD, nuklear sources with other sources
list(APPEND SOURCES ${CJSON_SOURCES} ${GLAD_SOURCES} ${NUKLEAR_SOURCES})

# Define the executable
add_executable(StellAI ${SOURCES})

# Link libraries
target_link_libraries(StellAI
    glfw             # GLFW for windowing
    glew             # GLEW for OpenGL extension loading
    SOIL2            # SOIL2 for image loading
    assimp           # Assimp for model loading
    tinyfiledialogs  # tinyfiledialogs for file dialogs
    GL               # OpenGL library on Linux
    X11              # Required for GLFW on Linux
    pthread          # POSIX thread library
    Xrandr           # Required for GLFW on Linux
    Xi               # Required for GLFW on Linux
    dl               # Dynamic loading library
)


# Explicitly specify the include directories
target_include_directories(StellAI PRIVATE 
    ${CMAKE_SOURCE_DIR}/external/glad/include      # Path to glad headers
    ${CMAKE_SOURCE_DIR}/external/glad/include/KHR  # Path to KHR headers for khrplatform.h
    ${GLFW_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIRS}
    ${SOIL2_INCLUDE_DIRS}
    ${ASSIMP_INCLUDE_DIRS}
    ${TINYFILEDIALOGS_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/include                    # Project's include directory
    ${CMAKE_SOURCE_DIR}/external/nuklear           # Nuklear as a header-only library
    ${CMAKE_SOURCE_DIR}/external/cjson             # cJSON include directory
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

# Ensure Nuklear and GLFW integration is handled correctly
if (EXISTS "${CMAKE_SOURCE_DIR}/external/nuklear/nuklear_glfw_gl3.h")
    target_include_directories(StellAI PRIVATE ${CMAKE_SOURCE_DIR}/external/nuklear)
else()
    message(FATAL_ERROR "Nuklear GLFW integration header file nuklear_glfw_gl3.h not found!")
endif()
