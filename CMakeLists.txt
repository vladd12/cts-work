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

# Setup build type if it's not specified
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
  message(STATUS "CMAKE_BUILD_TYPE is not specified. Using default: ${CMAKE_BUILD_TYPE}")
  # Possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

set(CMAKE_BUILD_TYPE Release)

# Getting third party libraries
include(get_glfw)           # Getting GLFW library
include(get_glew)           # Getting GLEW library
include(get_conan)          # Getting CMake conan
include(get_nlohmann_json)  # Getting nlohmann_json library

# Find required packages
find_package(OpenGL REQUIRED)
#find_package(GLEW REQUIRED)
#find_package(OpenCV REQUIRED)

# Build dependencies
add_subdirectory(deps)
# Build main executable target (application)
add_subdirectory(app)

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
