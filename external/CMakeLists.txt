cmake_minimum_required(VERSION 3.21)
project(external)

add_library(external INTERFACE)

add_subdirectory(spdlog)
target_link_libraries(external INTERFACE spdlog::spdlog)
target_include_directories(external INTERFACE spdlog::spdlog)

add_subdirectory(SFML)
target_link_libraries(external INTERFACE sfml-window sfml-graphics sfml-system sfml-audio sfml-network)
target_include_directories(external INTERFACE sfml-window sfml-graphics sfml-system sfml-audio sfml-network)

set(JSON_BuildTests OFF CACHE INTERNAL "")
set(JSON_Install OFF CACHE INTERNAL "")
add_subdirectory(json)
target_link_libraries(external INTERFACE nlohmann_json::nlohmann_json)
