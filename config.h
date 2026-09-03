#ifndef CONFIG_H
#define CONFIG_H

// General Configuration
#define TARGET_OS "Linux"
#define ARCHITECTURE "x86_64"
#define BUILD_DATE "2026-09-02"

// Compilation Flags
#define DEBUG 0
#define OPTIMIZE 2
#define ENABLE_ASSERTIONS false

// Network Configuration
#define MAX_CONNECTIONS 5
#define TIMEOUT 10  // seconds

// File System Configuration
#define TEMP_DIR "/tmp/.stealth"
#define CONFIG_FILE "/etc/.stealth/config"

#endif // CONFIG_H