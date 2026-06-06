@echo off
setlocal EnableExtensions

set "PROJECT=%~dp0"
if "%PROJECT:~-1%"=="\" set "PROJECT=%PROJECT:~0,-1%"
set "HTML=%PROJECT%\tb_cpp_explanation.html"

if not exist "%HTML%" (
    echo [FAIL] Not found:
    echo   %HTML%
    pause
    exit /b 1
)

start "" "%HTML%"
exit /b 0
