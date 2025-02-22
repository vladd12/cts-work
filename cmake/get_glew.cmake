# Getting GLEW sources from GitHub repository
FetchContent_Declare(
    glew
    GIT_REPOSITORY https://github.com/Perlmint/glew-cmake.git
    GIT_TAG        f456deace7b408655109aaeff71421ef2d3858c6 # v2.2.0
)

# Compiling GLEW library
FetchContent_MakeAvailable(glew)

# Import GLEW static library as glew::glew package
add_library(glew::glew INTERFACE IMPORTED)
target_link_libraries(glew::glew INTERFACE libglew_static)
