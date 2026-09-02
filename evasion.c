#include "evasion.h"
#include "payload_config.h"
#include <windows.h>
#include <stdio.h>

int RunEvasionChecks() {
	int result = 1;

#if ENABLE_ANTI_DEBUG
	if (IsDebuggerPresent()) {
		printf("Debugger detected!\n");
		result = 0;
	}
#endif

#if ENABLE_ANTI_VM
	// Simple VM detection using CPUID
	int cpuInfo[4];
	__cpuid(cpuInfo, 0x1);
	if (cpuInfo[2] & (1 << 31)) {
		printf("VM detected via CPUID!\n");
		result = 0;
	}
#endif

#if ENABLE_FILELESS
	// Check if we're running from memory
	char path[MAX_PATH];
	if (GetModuleFileNameA(NULL, path, MAX_PATH) != 0) {
		printf("Running from file: %s\n", path);
		// For fileless execution, this should be empty or a system process
	}
#endif

	return result;
}