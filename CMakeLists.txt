cmake_minimum_required(VERSION 3.30)

project(MyRaylibGame CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# Fetch glm
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.1  # Use a specific version tag
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(glm)

# Fetch raylib
set(FETCHCONTENT_QUIET FALSE)  # So you see progress

FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 5.5                    # or "master" for bleeding edge
    GIT_SHALLOW TRUE
)

# Build raylib as static lib (recommended)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(raylib)

# Fetch FastNoise2
FetchContent_Declare(
    FastNoise2
    GIT_REPOSITORY https://github.com/Auburn/FastNoise2.git
    GIT_TAG        master # Use a stable tag or "master"
    GIT_SHALLOW    TRUE
)

# Disable FastNoise2's internal testing and tool builds to save time
set(FASTNOISE2_NO_TESTS ON CACHE BOOL "" FORCE)
set(FASTNOISE2_NO_TOOLS ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(FastNoise2)

# Your executable
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS "${SRC_DIR}/*.cpp")
add_executable(${PROJECT_NAME} ${SRC_FILES})

# Link raylib
target_link_libraries(${PROJECT_NAME} PRIVATE raylib glm FastNoise2)

# Platform-specific dependencies (very important!)
if(APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE "-framework IOKit" "-framework Cocoa" "-framework OpenGL")
elseif(UNIX)
    target_link_libraries(${PROJECT_NAME} PRIVATE m pthread dl GL)
elseif(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE winmm gdi32 opengl32)
endif()

# Optional: nice output folder
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

target_compile_options(${PROJECT_NAME} PRIVATE
    -march=native          # Optimize for your current CPU
    -ffast-math            # Faster floating point (use with care in games)
    -flto                  # Link-time optimization (can make builds slower but faster runtime)
)