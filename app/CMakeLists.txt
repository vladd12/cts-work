# Main executable
add_executable(EdgeResponseAnalyzer
    main.cpp
    functions.cpp
    functions.h
)

# Link libraries
target_link_libraries(EdgeResponseAnalyzer
    ${OPENGL_LIBRARIES}
    ${GLEW_LIBRARIES}
#    ${OpenCV_LIBS}
    imgui
    tinyfiledialogs
    nlohmann_json::nlohmann_json
)

# Additional includes for the executable (PRIVATE as they are already PUBLIC for imgui and found packages)
target_include_directories(EdgeResponseAnalyzer PRIVATE
#    ${IMGUI_DIR}
#    ${IMGUI_DIR}/backends
    ${OPENGL_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIRS}
#    ${glfw3_INCLUDE_DIRS}
    ${OpenCV_INCLUDE_DIRS}
#    ${TINYFILEDIALOGS_DIR}
#    ${nlohmann_json_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}
)

# Output directory
set_target_properties(EdgeResponseAnalyzer PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CUURENT_BINARY_DIR}/bin
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CUURENT_BINARY_DIR}/bin/Release
)

# Setup build warnings
if(MSVC) # Keep MSVC specific warnings for potential future Windows build, remove if strictly Linux only
    target_compile_options(EdgeResponseAnalyzer PRIVATE /W4)
else()
    target_compile_options(EdgeResponseAnalyzer PRIVATE -Wall -Wextra)
endif()

# Create directories
file(MAKE_DIRECTORY ${CMAKE_CUURENT_BINARY_DIR}/bin)
file(MAKE_DIRECTORY ${CMAKE_CUURENT_BINARY_DIR}/bin/Release)

# Installation
install(TARGETS EdgeResponseAnalyzer
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)
