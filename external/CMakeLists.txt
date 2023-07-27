cmake_minimum_required(VERSION 3.27)
project(external)

add_library(external INTERFACE)

add_subdirectory(spdlog)
target_link_libraries(external INTERFACE spdlog::spdlog)
target_include_directories(external INTERFACE spdlog::spdlog)

add_subdirectory(SFML)

# SFML static
set(SFML_STATIC_LIBRARIES TRUE)
set(BUILD_SHARED_LIBS FALSE)

target_link_libraries(external INTERFACE sfml-window sfml-graphics sfml-system sfml-audio sfml-network)
target_include_directories(external INTERFACE sfml-window sfml-graphics sfml-system sfml-audio sfml-network)
