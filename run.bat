@echo off
echo ===================================================
echo  Building OpenGL Starter Project
echo ===================================================

:: Close any existing running instances to prevent file lock
taskkill /F /IM main.exe >nul 2>&1

g++ -std=c++17 src/main.cpp src/glad.c src/stb_image.cpp -Iinclude -Isrc -Llib -lglfw3 -lopengl32 -lgdi32 -luser32 -lshell32 -o main.exe

if %ERRORLEVEL% EQU 0 (
    echo.
    echo [BUILD SUCCESS] Launching main.exe...
    echo.
    main.exe
) else (
    echo.
    echo [BUILD FAILED] Check compilation errors above.
    pause
)
