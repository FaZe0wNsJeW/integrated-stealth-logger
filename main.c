#include "payload.h"
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

void handle_signal(int sig) {
    if (sig == SIGINT || sig == SIGTERM) {
        cleanup_payload();
        exit(0);
    }
}

int main() {
    // Set up signal handlers
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);
    
    // Initialize and run payload
    init_payload();
    run_payload();
    
    return 0;
}