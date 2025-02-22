# Getting nlohmann_json sources from GitHub repository
FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG        9cca280a4d0ccf0c08f47a99aa71d1b0e52f8d03 # v3.11.3
)

# Compiling nlohmann_json library
FetchContent_MakeAvailable(nlohmann_json)
