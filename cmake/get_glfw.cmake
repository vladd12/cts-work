# Getting GLFW sources from GitHub repository
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        7482de6071d21db77a7236155da44c172a7f6c9e # v3.3.8
)

# Check if population has already been performed
FetchContent_GetProperties(glfw)
if(NOT glfw_POPULATED)
  # Fetch the GLFW content using previously declared details
  FetchContent_Populate(glfw)

  # Changing some options for building GLFW without examples, test and docs
  option(GLFW_BUILD_EXAMPLES "Build the GLFW example programs" OFF)
  option(GLFW_BUILD_TESTS "Build the GLFW test programs" OFF)
  option(GLFW_BUILD_DOCS "Build the GLFW documentation" OFF)
  option(GLFW_INSTALL "Generate installation target" OFF)

  # Bring the populated GLFW content into the build
  add_subdirectory(${glfw_SOURCE_DIR} ${glfw_BINARY_DIR})
endif()

# Import GLFW library as glfw::glfw package
add_library(glfw::glfw INTERFACE IMPORTED)
target_link_libraries(glfw::glfw INTERFACE glfw)
