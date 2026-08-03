#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// API resolution functions using hash matching

// Simple hash function for API names
DWORD HashString(const char* szString) {
    DWORD dwHash = 0;
    while (*szString) {
        dwHash = (dwHash << 5) + dwHash + *szString;
        szString++;
    }
    return dwHash;
}

// Resolve API by hash
FARPROC ResolveAPIByHash(HMODULE hModule, DWORD dwHash) {
    if (!hModule) {
        return NULL;
    }

    PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)hModule;
    PIMAGE_NT_HEADERS pNtHeaders = (PIMAGE_NT_HEADERS)((BYTE*)hModule + pDosHeader->e_lfanew);
    
    if (pNtHeaders->Signature != IMAGE_NT_SIGNATURE) {
        return NULL;
    }

    PIMAGE_EXPORT_DIRECTORY pExportDir = (PIMAGE_EXPORT_DIRECTORY)((BYTE*)hModule + 
        pNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);

    PDWORD pNames = (PDWORD)((BYTE*)hModule + pExportDir->AddressOfNames);
    PDWORD pAddresses = (PDWORD)((BYTE*)hModule + pExportDir->AddressOfFunctions);
    PWORD pOrdinals = (PWORD)((BYTE*)hModule + pExportDir->AddressOfNameOrdinals);

    for (DWORD i = 0; i < pExportDir->NumberOfNames; i++) {
        const char* szFuncName = (const char*)((BYTE*)hModule + pNames[i]);
        DWORD dwCurrentHash = HashString(szFuncName);
        
        if (dwCurrentHash == dwHash) {
            return (FARPROC)((BYTE*)hModule + pAddresses[pOrdinals[i]]);
        }
    }

    return NULL;
}

// Resolve API by name (fallback)
FARPROC ResolveAPI(HMODULE hModule, LPCSTR lpProcName) {
    return GetProcAddress(hModule, lpProcName);
}
