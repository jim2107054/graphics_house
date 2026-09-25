@echo off
echo ===================================================
echo  Building Horror House at Night (Legacy OpenGL / GLUT)
echo  Developer: MD Jahid Hasan Jim (Roll: 2107054)
echo ===================================================

:: Close any existing running instances to prevent file lock
taskkill /F /IM main.exe >nul 2>&1

g++ -std=c++17 src/main.cpp -Iinclude -Llib -lfreeglut -lopengl32 -lglu32 -lgdi32 -lwinmm -o main.exe

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
