#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Persistence functions

BOOL AddToStartup(const char* szAppName, const char* szAppPath) {
    HKEY hKey;
    if (RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_SET_VALUE, &hKey) != ERROR_SUCCESS) {
        return FALSE;
    }
    
    if (RegSetValueEx(hKey, szAppName, 0, REG_SZ, (BYTE*)szAppPath, strlen(szAppPath) + 1) != ERROR_SUCCESS) {
        RegCloseKey(hKey);
        return FALSE;
    }
    
    RegCloseKey(hKey);
    return TRUE;
}

BOOL RemoveFromStartup(const char* szAppName) {
    HKEY hKey;
    if (RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_SET_VALUE, &hKey) != ERROR_SUCCESS) {
        return FALSE;
    }
    
    if (RegDeleteValue(hKey, szAppName) != ERROR_SUCCESS) {
        RegCloseKey(hKey);
        return FALSE;
    }
    
    RegCloseKey(hKey);
    return TRUE;
}
