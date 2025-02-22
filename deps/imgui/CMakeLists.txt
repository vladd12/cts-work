# ImGui library (static)
add_library(imgui STATIC
   ${CMAKE_CURRENT_SOURCE_DIR}/imgui.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/imgui_demo.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/imgui_draw.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/imgui_tables.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/imgui_widgets.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

# Build interface includes for ImGui library
target_include_directories(imgui PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR};${CMAKE_CURRENT_SOURCE_DIR}/backends>"
  "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

# Link dependencies for ImGui library
target_link_libraries(imgui PUBLIC glfw::glfw)
