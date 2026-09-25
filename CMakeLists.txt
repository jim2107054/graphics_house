cmake_minimum_required(VERSION 3.15)
project(HorrorHouseAtNight CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib)

set(SOURCES
    src/main.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

if (WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        freeglut
        glu32
        opengl32
        gdi32
        user32
        winmm
    )
elseif (APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        "-framework GLUT"
        "-framework OpenGL"
        "-framework Cocoa"
    )
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE
        glut
        GLU
        GL
        m
    )
endif()
