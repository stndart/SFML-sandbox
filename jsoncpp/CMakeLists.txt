cmake_minimum_required(VERSION 3.27)
project(jsoncpp)

# Set the path to the directory containing the JSONcpp source files
set(JSONCPP_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# Add the JSONcpp source files to the library target
add_library(jsoncpp_lib ${JSONCPP_SOURCE_DIR}/jsoncpp.cpp)

# Add an include directory for the library target to find the headers
target_include_directories(jsoncpp_lib PUBLIC ${JSONCPP_SOURCE_DIR}/..)

# If you want to enable C++20 support for JSONcpp, you can uncomment the following line
target_compile_features(jsoncpp_lib PUBLIC cxx_std_20)