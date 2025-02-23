# Find required packages
find_package(OpenCV REQUIRED)
if(NOT OpenCV_FOUND)
    message(FATAL_ERROR "OpenCV not found!")
endif()

# Main executable
add_executable(EdgeResponseAnalyzer
    main.cpp
    functions.cpp
    functions.h
)

# Link libraries
target_link_libraries(EdgeResponseAnalyzer
    opencv::opencv
    glew::glew
    imgui
    tinyfiledialogs
    nlohmann_json::nlohmann_json
)

# Additional includes for the executable (PRIVATE as they are already PUBLIC for imgui and found packages)
target_include_directories(EdgeResponseAnalyzer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Setup build warnings
if(MSVC) # Keep MSVC specific warnings for potential future Windows build, remove if strictly Linux only
    target_compile_options(EdgeResponseAnalyzer PRIVATE /W4)
else()
    target_compile_options(EdgeResponseAnalyzer PRIVATE -Wall -Wextra)
endif()

# Copy shaders
file(COPY
    ${CMAKE_CURRENT_SOURCE_DIR}/shaders/VertexShader.glsl
    ${CMAKE_CURRENT_SOURCE_DIR}/shaders/FragmentShader.glsl
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
)

# Installation
install(TARGETS EdgeResponseAnalyzer
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)
