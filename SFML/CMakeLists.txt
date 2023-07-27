cmake_minimum_required(VERSION 3.21)
project(SFML)

set(HEADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib)

add_library(SFML_lib::FLAC STATIC IMPORTED GLOBAL)
set_target_properties(SFML_lib::FLAC PROPERTIES IMPORTED_LOCATION ${LIB_DIR}/libFLAC.a)
add_library(SFML_lib::freetype STATIC IMPORTED GLOBAL)
set_target_properties(SFML_lib::freetype PROPERTIES IMPORTED_LOCATION ${LIB_DIR}/libfreetype.a)
add_library(SFML_lib::ogg STATIC IMPORTED GLOBAL)
set_target_properties(SFML_lib::ogg PROPERTIES IMPORTED_LOCATION ${LIB_DIR}/libogg.a)
add_library(SFML_lib::openal32 STATIC IMPORTED GLOBAL)
set_target_properties(SFML_lib::openal32 PROPERTIES IMPORTED_LOCATION ${LIB_DIR}/libopenal32.a)
add_library(SFML_lib::vorbis STATIC IMPORTED GLOBAL)
set_target_properties(SFML_lib::vorbis PROPERTIES IMPORTED_LOCATION ${LIB_DIR}/libvorbis.a)
add_library(SFML_lib::vorbisenc STATIC IMPORTED GLOBAL)
set_target_properties(SFML_lib::vorbisenc PROPERTIES IMPORTED_LOCATION ${LIB_DIR}/libvorbisenc.a)
add_library(SFML_lib::vorbisfile STATIC IMPORTED GLOBAL)
set_target_properties(SFML_lib::vorbisfile PROPERTIES IMPORTED_LOCATION ${LIB_DIR}/libvorbisfile.a)
if (CMAKE_BUILD_TYPE EQUAL "Debug")
    add_library(SFML_lib::sfml-graphics-s STATIC IMPORTED GLOBAL)
    set_target_properties(SFML_lib::sfml-graphics-s PROPERTIES IMPORTED_LOCATION ${LIB_DIR}/libsfml-graphics-s-d.a)
    add_library(SFML_lib::sfml-window-s STATIC IMPORTED GLOBAL)
    set_target_properties(SFML_lib::sfml-window-s PROPERTIES IMPORTED_LOCATION ${LIB_DIR}/libsfml-window-s-d.a)
    add_library(SFML_lib::sfml-system-s STATIC IMPORTED GLOBAL)
    set_target_properties(SFML_lib::sfml-system-s PROPERTIES IMPORTED_LOCATION ${LIB_DIR}/libsfml-system-s-d.a)
    add_library(SFML_lib::sfml-audio-s STATIC IMPORTED GLOBAL)
    set_target_properties(SFML_lib::sfml-audio-s PROPERTIES IMPORTED_LOCATION ${LIB_DIR}/libsfml-audio-s-d.a)
    add_library(SFML_lib::sfml-network-s STATIC IMPORTED GLOBAL)
    set_target_properties(SFML_lib::sfml-network-s PROPERTIES IMPORTED_LOCATION ${LIB_DIR}/libsfml-network-s-d.a)
    add_library(SFML_lib::sfml-main STATIC IMPORTED GLOBAL)
    set_target_properties(SFML_lib::sfml-main PROPERTIES IMPORTED_LOCATION ${LIB_DIR}/libsfml-main-d.a)
else()
    add_library(SFML_lib::sfml-graphics-s STATIC IMPORTED GLOBAL)
    set_target_properties(SFML_lib::sfml-graphics-s PROPERTIES IMPORTED_LOCATION ${LIB_DIR}/libsfml-graphics-s.a)
    add_library(SFML_lib::sfml-window-s STATIC IMPORTED GLOBAL)
    set_target_properties(SFML_lib::sfml-window-s PROPERTIES IMPORTED_LOCATION ${LIB_DIR}/libsfml-window-s.a)
    add_library(SFML_lib::sfml-system-s STATIC IMPORTED GLOBAL)
    set_target_properties(SFML_lib::sfml-system-s PROPERTIES IMPORTED_LOCATION ${LIB_DIR}/libsfml-system-s.a)
    add_library(SFML_lib::sfml-audio-s STATIC IMPORTED GLOBAL)
    set_target_properties(SFML_lib::sfml-audio-s PROPERTIES IMPORTED_LOCATION ${LIB_DIR}/libsfml-audio-s.a)
    add_library(SFML_lib::sfml-network-s STATIC IMPORTED GLOBAL)
    set_target_properties(SFML_lib::sfml-network-s PROPERTIES IMPORTED_LOCATION ${LIB_DIR}/libsfml-network-s.a)
    add_library(SFML_lib::sfml-main STATIC IMPORTED GLOBAL)
    set_target_properties(SFML_lib::sfml-main PROPERTIES IMPORTED_LOCATION ${LIB_DIR}/libsfml-main.a)
endif()

find_package(OpenGL REQUIRED)

add_library(SFML_lib INTERFACE)
target_link_libraries(SFML_lib INTERFACE
    SFML_lib::sfml-audio-s
    SFML_lib::sfml-network-s
    SFML_lib::sfml-main
    SFML_lib::sfml-graphics-s
    SFML_lib::sfml-window-s
    SFML_lib::sfml-system-s
    SFML_lib::FLAC
    SFML_lib::ogg
    SFML_lib::openal32
    SFML_lib::vorbis
    SFML_lib::vorbisenc
    SFML_lib::vorbisfile
    SFML_lib::freetype
    ${OPENGL_LIBRARIES}
    winmm
)

target_compile_definitions(SFML_lib INTERFACE SFML_STATIC)
target_include_directories(SFML_lib INTERFACE ${HEADER_DIR})
target_compile_features(SFML_lib INTERFACE cxx_std_20)