#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

// Updated EDR detection function using process enumeration
BOOL IsEDRDetected() {
    // List of common EDR process names
    const char* szEDRProcesses[] = {
        "msmpeng.exe",      // Microsoft Defender
        "mpcmdrun.exe",     // Microsoft Defender
        "msseces.exe",      // Microsoft Defender
        "smc.exe",          // Symantec Endpoint Protection
        "ccSvcHst.exe",     // Symantec Endpoint Protection
        "sepengine.exe",    // Symantec Endpoint Protection
        "mcshield.exe",     // McAfee
        "mfemms.exe",       // McAfee
        "mfevtps.exe",      // McAfee
        "avp.exe",          // Kaspersky
        "kav.exe",          // Kaspersky
        "kavsvc.exe",       // Kaspersky
        "avgemc.exe",       // AVG
        "avgnt.exe",        // AVG
        "avgupsvc.exe",     // AVG
        "aswidsagenta.exe", // Avast
        "avastsvc.exe",     // Avast
        "avastui.exe",      // Avast
        "esetnod32.exe",    // ESET NOD32
        "ekrn.exe",         // ESET NOD32
        "egui.exe",         // ESET NOD32
        "sophos.exe",       // Sophos
        "savservice.exe",   // Sophos
        "savadmin.exe",     // Sophos
        "tray.exe",         // Trend Micro
        "ntrtscan.exe",     // Trend Micro
        "tmproxy.exe",      // Trend Micro
        "cylance.exe",      // Cylance
        "cylancesvc.exe",   // Cylance
        "cylanceui.exe",    // Cylance
        "carbonblack.exe",  // Carbon Black
        "cb.exe",           // Carbon Black
        "cbsvc.exe",        // Carbon Black
        "crowdstrike.exe",  // CrowdStrike
        "csagent.exe",      // CrowdStrike
        "csfalcon.exe",     // CrowdStrike
        "tanium.exe",       // Tanium
        "taniumclient.exe", // Tanium
        "taniumsvc.exe",    // Tanium
        "sentinelone.exe",  // SentinelOne
        "sentinelagent.exe",// SentinelOne
        "sentinelui.exe",   // SentinelOne
        "paloaltonetworks.exe", // Palo Alto Networks
        "pan.exe",          // Palo Alto Networks
        "pansvc.exe",       // Palo Alto Networks
        "fireeye.exe",      // FireEye
        "fems.exe",         // FireEye
        "fesvc.exe",        // FireEye
        "mandiant.exe",     // Mandiant
        "mdi.exe",          // Mandiant
        "mdischedule.exe",  // Mandiant
        NULL
    };

    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        return FALSE;
    }

    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    if (!Process32First(hSnapshot, &pe32)) {
        CloseHandle(hSnapshot);
        return FALSE;
    }

    do {
        // Convert process name to lowercase for case-insensitive comparison
        char szProcessName[MAX_PATH];
        strncpy(szProcessName, pe32.szExeFile, sizeof(szProcessName) - 1);
        szProcessName[sizeof(szProcessName) - 1] = '\0';
        
        for (int i = 0; szEDRProcesses[i] != NULL; i++) {
            if (_stricmp(szProcessName, szEDRProcesses[i]) == 0) {
                CloseHandle(hSnapshot);
                return TRUE;
            }
        }
    } while (Process32Next(hSnapshot, &pe32));

    CloseHandle(hSnapshot);
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
    
    if (IsEDRDetected()) {
        printf("EDR detected! Exiting...\n");
        exit(1);
    }
}
