# Downloading conan CMake script and using for autodetecting
if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/conan.cmake")
  message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
  file(
    DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/0.18.1/conan.cmake" "${CMAKE_CURRENT_BINARY_DIR}/conan.cmake"
    EXPECTED_HASH SHA256=5cdb3042632da3efff558924eecefd580a0e786863a857ca097c3d1d43df5dcd
    TLS_VERIFY ON
  )
endif()
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_BINARY_DIR}")
include(${CMAKE_CURRENT_BINARY_DIR}/conan.cmake)
