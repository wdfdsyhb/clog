@echo off
rem Build and run the clog self-test with MSVC.
rem Run from a Visual Studio Developer Command Prompt (where cl is on PATH).
where cl >nul 2>nul
if errorlevel 1 (
    echo error: cl.exe not found - open a VS Developer Command Prompt first
    exit /b 1
)
cl /nologo /W4 /O2 clog.c test_clog.c /Fe:clog_test.exe
if errorlevel 1 exit /b 1
clog_test.exe
