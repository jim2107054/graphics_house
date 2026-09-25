Write-Host "===================================================" -ForegroundColor Cyan
Write-Host " Building Horror House at Night (Legacy OpenGL/GLUT)" -ForegroundColor Cyan
Write-Host " Developer: MD Jahid Hasan Jim (Roll: 2107054)" -ForegroundColor Yellow
Write-Host "===================================================" -ForegroundColor Cyan

# Close any running instance of main.exe
Get-Process -Name "main" -ErrorAction SilentlyContinue | Stop-Process -Force

# Compile with g++
& g++ -std=c++17 src/main.cpp -Iinclude -Llib -lfreeglut -lopengl32 -lglu32 -lgdi32 -lwinmm -o main.exe

if ($LASTEXITCODE -eq 0) {
    Write-Host "`n[BUILD SUCCESS] Launching main.exe...`n" -ForegroundColor Green
    & .\main.exe
} else {
    Write-Host "`n[BUILD FAILED] Check compilation errors above." -ForegroundColor Red
}
