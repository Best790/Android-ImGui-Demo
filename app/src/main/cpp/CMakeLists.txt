cmake_minimum_required(VERSION 3.22.1)
project(imguidemo LANGUAGES CXX)
add_library(imguidemo SHARED native-lib.cpp)
find_library(log-lib log)
find_library(android-lib android)
find_library(gles-lib GLESv3)
target_link_libraries(imguidemo ${log-lib} ${android-lib} ${gles-lib})
# Add upstream Dear ImGui source files to app/src/main/cpp/imgui.
set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/imgui)
if(EXISTS "${IMGUI_DIR}/imgui.cpp")
  target_sources(imguidemo PRIVATE
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_android.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp)
  target_include_directories(imguidemo PRIVATE ${IMGUI_DIR} ${IMGUI_DIR}/backends)
  target_compile_definitions(imguidemo PRIVATE IMGUI_IMPL_OPENGL_ES3)
else()
  message(FATAL_ERROR "Dear ImGui source missing. Follow README.md to clone upstream into app/src/main/cpp/imgui.")
endif()
