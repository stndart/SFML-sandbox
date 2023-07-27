cmake_minimum_required(VERSION 3.21)
project(SFML_sandbox)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")

add_subdirectory(external)
#add_subdirectory(test)

file(GLOB SOURCES "*.cpp")

add_executable(SFML_sandbox ${SOURCES})

target_compile_options(SFML_sandbox PUBLIC -Wall)
target_compile_features(SFML_sandbox PUBLIC cxx_std_20)

target_link_libraries(SFML_sandbox PUBLIC external)
target_include_directories(SFML_sandbox PUBLIC external)

#target_link_libraries(SFML_sandbox PUBLIC test)
#target_include_directories(SFML_sandbox PUBLIC test)

set_target_properties(SFML_sandbox PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})