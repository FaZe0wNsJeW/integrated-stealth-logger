#ifndef CONFIG_H
#define CONFIG_H

// General Configuration
#define DEBUG 0
#define VERBOSE 0
#define LOG_LEVEL "INFO"

// Network Configuration
#define MAX_CONNECTIONS 10
#define BUFFER_SIZE 4096
#define TIMEOUT 30 // seconds

// File Configuration
#define MAX_FILE_SIZE 1024 * 1024 * 10 // 10MB
#define TEMP_DIR "/tmp"

// Process Configuration
#define MAX_PROCESSES 50
#define PROCESS_INTERVAL 60 // seconds

#endif // CONFIG_H