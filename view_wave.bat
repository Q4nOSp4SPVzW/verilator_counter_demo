@echo off
setlocal EnableExtensions

rem ================================================================
rem view_wave.bat -- open counter_trace.vcd with GTKWave via WSL
rem
rem Usage:
rem   view_wave.bat          generate VCD if missing, then open GTKWave
rem   view_wave.bat rebuild  rerun Verilator first, then open GTKWave
rem ================================================================

set "PROJECT=%~dp0"
if "%PROJECT:~-1%"=="\" set "PROJECT=%PROJECT:~0,-1%"
set "DISTRO=Ubuntu-22.04"
set "VCD=%PROJECT%\counter_trace.vcd"
set "GTKW=%PROJECT%\counter_trace.gtkw"

cd /d "%PROJECT%"

echo.
echo ================================================================
echo   Verilator counter waveform viewer
echo ================================================================

if /i "%~1"=="rebuild" goto RUN_SIM
if not exist "%VCD%" goto RUN_SIM
goto OPEN_WAVE

:RUN_SIM
echo.
echo Generating waveform with Verilator...
wsl -d %DISTRO% -- bash -lc "cd /mnt/c/Users/skenc/.efinity/project/verilator_counter_demo && bash run_wsl.sh"
if errorlevel 1 (
    echo.
    echo [FAIL] Verilator simulation failed.
    pause
    exit /b 1
)

if not exist "%VCD%" (
    echo.
    echo [FAIL] VCD was not generated:
    echo   %VCD%
    pause
    exit /b 1
)

:OPEN_WAVE
echo.
echo Opening GTKWave:
echo   %GTKW%
echo.
wsl -d %DISTRO% -- bash -lc "cd /mnt/c/Users/skenc/.efinity/project/verilator_counter_demo && command -v gtkwave >/dev/null 2>&1 && gtkwave counter_trace.gtkw || { echo 'gtkwave is not installed. Run: sudo apt-get install -y gtkwave'; exit 1; }"
if errorlevel 1 (
    echo.
    echo [FAIL] GTKWave could not be started.
    pause
    exit /b 1
)

exit /b 0
