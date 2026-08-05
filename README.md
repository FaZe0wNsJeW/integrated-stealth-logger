# Integrated Stealth Logger

A modular, stealthy post-exploitation framework for Windows systems.

## Features

- **Modular Architecture**: Load only the modules you need
- **Command-Line Interface**: Easy to use with simple syntax
- **Stealthy Operations**: Designed to avoid detection by EDR/XDR systems
- **Extensible**: Add new modules without recompiling the entire framework

## Usage

```bash
stealthlogger -p <module1,module2,...>
```

### Module IDs

1. **Credential Harvester** - Extracts credentials from system
2. **Lateral Movement** - Performs lateral movement attacks
3. **File Transfer** - Transfers files between target and C2
4. **Process Injection** - Injects payloads into remote processes
5. **System Information** - Gathers detailed system data
6. **ETW/AMSI Patcher** - Patches ETW and AMSI in memory
7. **Call Stack Spoofer** - Implements call stack spoofing techniques
8. **Keylogger** - Captures keystrokes with timestamps
9. **Screenshot** - Periodically captures desktop screenshots
10. **Network Scanner** - Maps local network and identifies vulnerable hosts

### Examples

```bash
# Load credential harvester and file transfer modules
stealthlogger -p 1,3

# Load all evasion modules
stealthlogger -p 6,7

# Load complete post-exploitation suite
stealthlogger -p 1,2,3,4,5,8,9,10
```

## Compilation

```bash
# Cross-compile from Linux
make

# Compile on Windows with MinGW
gcc -Os -s -o stealthlogger.exe main.c modular_payload.c
```

## Notes

- This tool is for educational purposes only
- Use only on systems you own or have explicit permission to test
- The authors are not responsible for any misuse or damage caused by this tool

## License

MIT License - see LICENSE file for details