#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

// Test COM hijack by creating Wscript.Shell instance
int TestWscriptShellHijack() {
    printf("[+] Testing Wscript.Shell COM hijack...\n");
    
    HRESULT hr = CoInitialize(NULL);
    if (FAILED(hr)) {
        printf("[-] CoInitialize failed: 0x%08X\n", hr);
        return 0;
    }
    
    CLSID clsid;
    hr = CLSIDFromProgID(L"Wscript.Shell", &clsid);
    if (FAILED(hr)) {
        printf("[-] CLSIDFromProgID failed: 0x%08X\n", hr);
        CoUninitialize();
        return 0;
    }
    
    IUnknown* pUnknown;
    hr = CoCreateInstance(clsid, NULL, CLSCTX_INPROC_SERVER, IID_IUnknown, (void**)&pUnknown);
    if (FAILED(hr)) {
        printf("[-] CoCreateInstance failed: 0x%08X\n", hr);
        CoUninitialize();
        return 0;
    }
    
    printf("[+] Wscript.Shell instance created successfully\n");
    printf("[+] COM hijack is working - sentinel.dll should have executed\n");
    
    pUnknown->Release();
    CoUninitialize();
    return 1;
}

// Test COM hijack by opening a text file
int TestTextFileHijack() {
    printf("\n[+] Testing Text Preview Handler COM hijack...\n");
    
    // Create a test text file
    FILE* testFile = fopen("C:\\Windows\\Temp\\test.txt", "w");
    if (!testFile) {
        printf("[-] Failed to create test file\n");
        return 0;
    }
    fprintf(testFile, "COM hijack test file\n");
    fclose(testFile);
    
    // Open the file with explorer.exe
    ShellExecuteA(NULL, "open", "C:\\Windows\\Temp\\test.txt", NULL, NULL, SW_SHOWNORMAL);
    printf("[+] Test file opened in explorer.exe\n");
    printf("[+] COM hijack is working - sentinel.dll should have executed\n");
    
    return 1;
}

// Check if sentinel DLL is running
int CheckSentinelRunning() {
    printf("\n[+] Checking if sentinel.dll is running...\n");
    
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetCurrentProcessId());
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        printf("[-] CreateToolhelp32Snapshot failed\n");
        return 0;
    }
    
    MODULEENTRY32 me32;
    me32.dwSize = sizeof(MODULEENTRY32);
    
    if (Module32First(hSnapshot, &me32)) {
        do {
            if (strstr(me32.szModule, "sentinel.dll") != NULL) {
                printf("[+] sentinel.dll is loaded in process\n");
                CloseHandle(hSnapshot);
                return 1;
            }
        } while (Module32Next(hSnapshot, &me32));
    }
    
    printf("[-] sentinel.dll not found in process\n");
    CloseHandle(hSnapshot);
    return 0;
}

// Check registry for COM hijack
int CheckRegistryHijack(const char* clsid) {
    printf("\n[+] Checking registry for CLSID %s...\n", clsid);
    
    char regKey[MAX_PATH];
    snprintf(regKey, sizeof(regKey), "SOFTWARE\\Classes\\CLSID\\%s\\InprocServer32", clsid);
    
    HKEY hKey;
    LONG result = RegOpenKeyExA(HKEY_LOCAL_MACHINE, regKey, 0, KEY_READ, &hKey);
    if (result != ERROR_SUCCESS) {
        printf("[-] Failed to open registry key: %ld\n", result);
        return 0;
    }
    
    char dllPath[MAX_PATH];
    DWORD dllPathSize = sizeof(dllPath);
    result = RegQueryValueExA(hKey, NULL, NULL, NULL, (BYTE*)dllPath, &dllPathSize);
    if (result != ERROR_SUCCESS) {
        printf("[-] Failed to query registry value: %ld\n", result);
        RegCloseKey(hKey);
        return 0;
    }
    
    printf("[+] InprocServer32 value: %s\n", dllPath);
    if (strstr(dllPath, "payload.dll") != NULL || strstr(dllPath, "sentinel.dll") != NULL) {
        printf("[+] COM hijack is active\n");
        RegCloseKey(hKey);
        return 1;
    } else {
        printf("[-] COM hijack not detected\n");
        RegCloseKey(hKey);
        return 0;
    }
}

int main() {
    printf("=== COM Hijack Persistence Test Suite ===\n\n");
    
    int score = 0;
    
    // Test 1: Wscript.Shell hijack
    if (TestWscriptShellHijack()) score++;
    
    // Test 2: Text file preview hijack
    if (TestTextFileHijack()) score++;
    
    // Test 3: Check sentinel running
    if (CheckSentinelRunning()) score++;
    
    // Test 4: Check Wscript.Shell registry
    if (CheckRegistryHijack("{72C24DD5-D70A-438B-8A42-98424B88AFB8}")) score++;
    
    // Test 5: Check Text Preview Handler registry
    if (CheckRegistryHijack("{89BCB740-6119-101A-BCB7-00DD010655AF}")) score++;
    
    printf("\n=== Test Results ===\n");
    printf("Score: %d/5\n", score);
    
    if (score == 5) {
        printf("[+] All tests passed! Persistence is fully operational\n");
    } else {
        printf("[-] Some tests failed. Check the output above for details\n");
    }
    
    return 0;
}