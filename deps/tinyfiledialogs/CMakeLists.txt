# TinyFileDialogs library (static)
add_library(tinyfiledialogs STATIC
   ${CMAKE_CURRENT_SOURCE_DIR}/tinyfiledialogs.c
)

# Build interface includes for TinyFileDialogs library
target_include_directories(tinyfiledialogs PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
  "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")
