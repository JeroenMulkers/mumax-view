cmake_minimum_required(VERSION 3.10)

find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm REQUIRED)
find_package(OpenGL REQUIRED)

add_executable(myapp 
    main.cpp
)

target_include_directories(myapp PUBLIC ${OPENGL_INCLUDE_DIR} ${GLEW_INCLUDE_DIR} ${GLM_INCLUDE_DIR})
target_link_libraries(myapp glfw ${GLEW_LIBRARIES} ${OPENGL_LIBRARIES} ${GLM_LIBRARIES})