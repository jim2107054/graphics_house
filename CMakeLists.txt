cmake_minimum_required(VERSION 3.15)
project(OpenGLStarter C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include header paths (GLAD, GLFW, GLM, stb_image)
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Static and dynamic library search path
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib)

# Source files
set(SOURCES
    src/main.cpp
    src/glad.c
    src/stb_image.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

# Link platform and OpenGL libraries
if (WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        glfw3
        opengl32
        gdi32
        user32
        shell32
    )
elseif (APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        glfw
        "-framework OpenGL"
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreVideo"
    )
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE
        glfw
        GL
        X11
        pthread
        Xrandr
        Xi
        dl
    )
endif()

# On Windows, copy glfw3.dll if it exists in the root
if (WIN32 AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/glfw3.dll")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/glfw3.dll $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )
endif()
