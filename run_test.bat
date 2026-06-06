@echo off
setlocal EnableExtensions

rem ================================================================
rem run_test.bat -- run Verilator counter test on WSL
rem
rem Usage:
rem   run_test.bat
rem ================================================================

set "PROJECT=%~dp0"
if "%PROJECT:~-1%"=="\" set "PROJECT=%PROJECT:~0,-1%"
set "DISTRO=Ubuntu-22.04"

cd /d "%PROJECT%"

echo.
echo ================================================================
echo   Verilator counter test
echo ================================================================
echo   Windows: %PROJECT%
echo   WSL    : /mnt/c/Users/skenc/.efinity/project/verilator_counter_demo
echo.

wsl -d %DISTRO% -- bash -lc "cd /mnt/c/Users/skenc/.efinity/project/verilator_counter_demo && bash run_wsl.sh"
set "RESULT=%ERRORLEVEL%"

echo.
if "%RESULT%"=="0" (
    echo [PASS] Verilator counter test completed.
    echo   Coverage HTML: %PROJECT%\html\index.html
    echo   Waveform     : %PROJECT%\counter_trace.vcd
    echo   GTKWave list : %PROJECT%\counter_trace.gtkw
) else (
    echo [FAIL] Verilator counter test failed. exit=%RESULT%
)
echo.

pause
exit /b %RESULT%
