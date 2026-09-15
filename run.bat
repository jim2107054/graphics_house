@echo off
echo ===================================================
echo  Building Cinematic Halloween Haunted House (OpenGL)
echo ===================================================

g++ -std=c++17 src/main.cpp src/glad.c -Iinclude -Isrc -Llib -lglfw3 -lopengl32 -lgdi32 -luser32 -lshell32 -o HauntedHouse.exe

if %ERRORLEVEL% EQU 0 (
    echo.
    echo [BUILD SUCCESS] Launching HauntedHouse.exe...
    echo.
    HauntedHouse.exe
) else (
    echo.
    echo [BUILD FAILED] Check compilation errors above.
    pause
)
