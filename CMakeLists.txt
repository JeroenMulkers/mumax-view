cmake_minimum_required(VERSION 3.5.1)

project(vimag)

file(GLOB SOURCES src/*)
add_executable(vimag ${SOURCES})
add_definitions(-std=c++14)

if (EMSCRIPTEN) 

    set_target_properties(vimag PROPERTIES LINK_FLAGS " \
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

else(EMSCRIPTEN)

    find_package(OpenGL REQUIRED)
    find_package(glfw3 REQUIRED)

    target_include_directories(vimag PUBLIC ${OPENGL_INCLUDE_DIR})
    target_link_libraries(vimag ${OPENGL_LIBRARIES})
    target_link_libraries(vimag glfw)

endif(EMSCRIPTEN)