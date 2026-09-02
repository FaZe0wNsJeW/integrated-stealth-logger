#ifndef CONFIG_H
#define CONFIG_H

// General configuration
#define CONFIG_VERSION "1.0.0"
#define CONFIG_NAME "StealthLoggerConfig"

// Build configuration
#define BUILD_TYPE "RELEASE"
#define BUILD_DATE __DATE__
#define BUILD_TIME __TIME__

// System configuration
#define SYSTEM_LOG_DIR "/var/log"
#define SYSTEM_TEMP_DIR "/tmp"
#define SYSTEM_USER_DIR "/home"

// Network configuration
#define NETWORK_TIMEOUT 10 // seconds
#define NETWORK_MAX_RETRIES 3

// Security configuration
#define ENABLE_ENCRYPTION 1
#define ENABLE_COMPRESSION 1
#define ENABLE_INTEGRITY_CHECK 1

#endif // CONFIG_H
