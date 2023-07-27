cmake_minimum_required(VERSION 3.27)
project(SFML_sandbox)

set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")

add_subdirectory(jsoncpp)
add_subdirectory(SFML)
#add_subdirectory(test)

file(GLOB SOURCES "*.cpp")

add_executable(SFML_sandbox ${SOURCES})

target_compile_options(SFML_sandbox PUBLIC -Wall -Werror)
target_compile_features(SFML_sandbox PUBLIC cxx_std_20)

target_link_libraries(SFML_sandbox PUBLIC jsoncpp_lib)
#target_link_libraries(SFML_sandbox PUBLIC test)

target_include_directories(SFML_sandbox PUBLIC jsoncpp_lib)
target_include_directories(SFML_sandbox PUBLIC spdlog)
target_include_directories(SFML_sandbox PUBLIC SFML/include)
#target_include_directories(SFML_sandbox PUBLIC test)

target_link_libraries(SFML_sandbox PUBLIC SFML_lib)

set_target_properties(SFML_sandbox PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})