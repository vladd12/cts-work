cmake_minimum_required(VERSION 3.10)
project(EdgeResponseAnalyzer)

# C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find required packages
find_package(OpenGL REQUIRED)
find_package(OpenCV REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED) # Use glfw3 to align with find_package module name
find_package(nlohmann_json REQUIRED) # Using nlohmann_json as a common JSON library, adjust if you use a different one

if(NOT OpenCV_FOUND)
    message(FATAL_ERROR "OpenCV not found!")
endif()
if(NOT OpenGL_FOUND)
    message(FATAL_ERROR "OpenGL not found!")
endif()
if(NOT GLEW_FOUND)
    message(FATAL_ERROR "GLEW not found!")
endif()
if(NOT glfw3_FOUND)
    message(FATAL_ERROR "GLFW not found!")
endif()
if(NOT nlohmann_json_FOUND)
    message(FATAL_ERROR "nlohmann_json not found!")
endif()

# ImGui directory (assuming it's in deps/imgui relative to source root)
set(IMGUI_DIR "${CMAKE_SOURCE_DIR}/deps/imgui")
set(TINYFILEDIALOGS_DIR "${CMAKE_SOURCE_DIR}/deps")

# Include directories
include_directories(
    ${OPENGL_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIRS}
    ${glfw3_INCLUDE_DIRS}
    ${OpenCV_INCLUDE_DIRS}
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    ${TINYFILEDIALOGS_DIR}
    ${nlohmann_json_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}
)

# ImGui library (static)
add_library(imgui STATIC
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

target_include_directories(imgui PUBLIC
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    ${glfw3_INCLUDE_DIRS}
)

# Main executable
add_executable(EdgeResponseAnalyzer
    main.cpp
    functions.cpp
    functions.h
    ${TINYFILEDIALOGS_DIR}/tinyfiledialogs.c
)

# Link libraries
target_link_libraries(EdgeResponseAnalyzer
    ${OPENGL_LIBRARIES}
    ${GLEW_LIBRARIES}
    glfw # Use glfw3 target name from find_package
    ${OpenCV_LIBS}
    imgui
)

# Additional includes for the executable (PRIVATE as they are already PUBLIC for imgui and found packages)
target_include_directories(EdgeResponseAnalyzer PRIVATE
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    ${OPENGL_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIRS}
    ${glfw3_INCLUDE_DIRS}
    ${OpenCV_INCLUDE_DIRS}
    ${TINYFILEDIALOGS_DIR}
    ${nlohmann_json_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}
)

# Copy shaders
file(COPY
    ${CMAKE_SOURCE_DIR}/VertexShader.glsl
    ${CMAKE_SOURCE_DIR}/FragmentShader.glsl
    DESTINATION ${CMAKE_BINARY_DIR}/bin/Release
)
file(COPY
    ${CMAKE_SOURCE_DIR}/VertexShader.glsl
    ${CMAKE_SOURCE_DIR}/FragmentShader.glsl
    DESTINATION ${CMAKE_BINARY_DIR}
)

# Output directory
set_target_properties(EdgeResponseAnalyzer PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin/Release
)

# Build type and warnings
set(CMAKE_BUILD_TYPE Release)
if(MSVC) # Keep MSVC specific warnings for potential future Windows build, remove if strictly Linux only
    target_compile_options(EdgeResponseAnalyzer PRIVATE /W4)
    target_compile_options(imgui PRIVATE /W4)
else()
    target_compile_options(EdgeResponseAnalyzer PRIVATE -Wall -Wextra)
    target_compile_options(imgui PRIVATE -Wall -Wextra)
endif()

# Create directories
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin/Release)

# Installation
install(TARGETS EdgeResponseAnalyzer
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)
