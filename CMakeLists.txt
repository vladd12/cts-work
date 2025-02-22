cmake_minimum_required(VERSION 3.20)
project(EdgeResponseAnalyzer)

# C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# CMake's fetch content options
include(FetchContent)
set(FETCHCONTENT_UPDATES_DISCONNECTED ON)

# Adding path for cmake scripts
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Setup CMake build type
set(CMAKE_BUILD_TYPE Release)

# Getting third party libraries
include(get_glfw)          # Getting GLFW library
include(get_conan)         # Getting CMake conan
include(get_nlohmann_json) # Getting nlohmann_json library

# Build dependencies
add_subdirectory(deps)
# Build main executable target (application)
add_subdirectory(app)

# Find required packages
find_package(OpenGL REQUIRED)
#find_package(glfw3 REQUIRED) # Use glfw3 to align with find_package module name
#find_package(GLEW REQUIRED)
#find_package(nlohmann_json REQUIRED) # Using nlohmann_json as a common JSON library, adjust if you use a different one
#find_package(OpenCV REQUIRED)

#if(NOT OpenCV_FOUND)
#    message(FATAL_ERROR "OpenCV not found!")
#endif()
#if(NOT OpenGL_FOUND)
#    message(FATAL_ERROR "OpenGL not found!")
#endif()
#if(NOT GLEW_FOUND)
#    message(FATAL_ERROR "GLEW not found!")
#endif()
#if(NOT glfw3_FOUND)
#    message(FATAL_ERROR "GLFW not found!")
#endif()
#if(NOT nlohmann_json_FOUND)
#    message(FATAL_ERROR "nlohmann_json not found!")
#endif()

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
