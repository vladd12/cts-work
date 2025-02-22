# Getting opencv target with conan
if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/FindOpenCV.cmake")
    # Getting build type, compiler, etc... (other settings)
    conan_cmake_autodetect(settings)
    # Collecting opencv
    conan_cmake_configure(
      REQUIRES opencv/4.10.0
      GENERATORS cmake_find_package
    )
    # Installing libraries
    conan_cmake_install(
      PATH_OR_REFERENCE .
      BUILD missing
      REMOTE conancenter
      SETTINGS ${settings}
    )
endif()