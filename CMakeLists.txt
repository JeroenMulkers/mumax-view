cmake_minimum_required(VERSION 3.5.1)
project(mumax-view)

set(CMAKE_CXX_STANDARD 17)

file(GLOB SOURCES src/*)
add_executable(mumax-view ${SOURCES})

set_target_properties(mumax-view PROPERTIES LINK_FLAGS " \
    --bind \
    -s WASM=1 \
    -s USE_GLFW=3 \
    -s USE_WEBGL2=1 \
    -s FORCE_FILESYSTEM=1 \
    -s EXPORT_ALL=1 \
    -s EXTRA_EXPORTED_RUNTIME_METHODS=['cwrap'] \
    -s ALLOW_MEMORY_GROWTH=1 ")

configure_file(
    ${CMAKE_SOURCE_DIR}/index.html 
    ${CMAKE_CURRENT_BINARY_DIR}/index.html 
    COPYONLY )