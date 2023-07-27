cmake_minimum_required(VERSION 3.27)

set(PROJECT_NAME SFML_sandbox)

project(${PROJECT_NAME})

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")

file(GLOB SOURCES "*.cpp")

add_executable(${PROJECT_NAME} ${SOURCES})

target_compile_options(${PROJECT_NAME} PUBLIC -Wall)# -Werror)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)

add_subdirectory(external)

target_link_libraries(${PROJECT_NAME} PUBLIC external)
target_include_directories(${PROJECT_NAME} PUBLIC external)

add_subdirectory(jsoncpp)
target_link_libraries(${PROJECT_NAME} PUBLIC jsoncpp_lib)
target_include_directories(${PROJECT_NAME} PUBLIC jsoncpp)

set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
