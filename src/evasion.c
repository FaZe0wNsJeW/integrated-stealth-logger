#include "evasion.h"
#include <stdio.h>
#include <windows.h>

int init_evasion() {
    printf("Initializing evasion techniques\n");
    
    // Anti-debug checks
    if (IsDebuggerPresent()) {
        printf("Debugger detected - exiting\n");
        return 0;
    }
    
    // Anti-sandbox checks
    if (check_sandbox()) {
        printf("Sandbox detected - exiting\n");
        return 0;
    }
    
    printf("Evasion techniques initialized successfully\n");
    return 1;
}

int check_sandbox() {
    // Simple sandbox detection checks
    if (GetTickCount() < 10000) {
        return 1;
    }
    
    return 0;
}

void stealth_sleep(int milliseconds) {
    // Stealthy sleep to avoid detection
    Sleep(milliseconds);
}