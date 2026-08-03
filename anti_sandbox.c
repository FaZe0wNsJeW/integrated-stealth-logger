#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Anti-sandbox detection functions

BOOL IsRunningInVM() {
    // Check for VM-specific registry keys
    HKEY hKey;
    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Control\\SystemInformation", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        RegCloseKey(hKey);
        return TRUE;
    }
    
    // Check for VM-specific files
    if (GetFileAttributes("C:\\Program Files\\VMware\\VMware Tools\\vmtoolsd.exe") != INVALID_FILE_ATTRIBUTES) {
        return TRUE;
    }
    
    if (GetFileAttributes("C:\\Program Files\\Oracle\\VirtualBox\\VBoxService.exe") != INVALID_FILE_ATTRIBUTES) {
        return TRUE;
    }
    
    return FALSE;
}

BOOL IsERDDetected() {
    // Check for Emergency Repair Disk environment
    char szPath[MAX_PATH];
    if (GetSystemDirectory(szPath, MAX_PATH)) {
        if (strstr(szPath, "ERD") != NULL || strstr(szPath, "REPAIR") != NULL) {
            return TRUE;
        }
    }
    return FALSE;
}

VOID AntiSandboxCheck() {
    if (IsRunningInVM()) {
        printf("VM detected! Exiting...\n");
        exit(1);
    }
    
    if (IsERDDetected()) {
        printf("ERD environment detected! Exiting...\n");
        exit(1);
    }
}
