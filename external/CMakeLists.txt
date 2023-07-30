cmake_minimum_required(VERSION 3.27)
project(external)

add_library(external INTERFACE)

set(BUILD_SHARED_LIBS FALSE)

add_subdirectory(spdlog)
target_link_libraries(external INTERFACE spdlog::spdlog)
target_include_directories(external INTERFACE spdlog::spdlog)

# SFML static
set(SFML_STATIC_LIBRARIES TRUE)

add_subdirectory(SFML)

target_link_libraries(external INTERFACE sfml-window sfml-graphics sfml-system sfml-audio sfml-network)
target_include_directories(external INTERFACE sfml-window sfml-graphics sfml-system sfml-audio sfml-network)

add_library(imgui imgui/imgui_draw.cpp imgui/imgui_tables.cpp imgui/imgui_widgets.cpp imgui/imgui.cpp imgui/imgui_demo.cpp)

target_compile_features(imgui PUBLIC cxx_std_20)

target_link_libraries(external INTERFACE imgui)
target_include_directories(external INTERFACE imgui)

set(IMGUI_DIR ../imgui)
set(IMGUI_SFML_FIND_SFML FALSE)

add_subdirectory(imgui-sfml)

target_link_libraries(external INTERFACE ImGui-SFML::ImGui-SFML)
target_include_directories(external INTERFACE ImGui-SFML::ImGui-SFML)

add_subdirectory(json)

target_link_libraries(external INTERFACE nlohmann_json::nlohmann_json)
target_include_directories(external INTERFACE nlohmann_json::nlohmann_json)