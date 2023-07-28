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
