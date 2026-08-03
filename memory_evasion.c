#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Memory evasion functions

BOOL AllocateStealthMemory(SIZE_T dwSize, LPVOID* ppAddress) {
    *ppAddress = VirtualAlloc(NULL, dwSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    return (*ppAddress != NULL);
}

BOOL ProtectStealthMemory(LPVOID pAddress, SIZE_T dwSize) {
    DWORD dwOldProtect;
    return VirtualProtect(pAddress, dwSize, PAGE_NOACCESS, &dwOldProtect);
}

BOOL UnprotectStealthMemory(LPVOID pAddress, SIZE_T dwSize) {
    DWORD dwOldProtect;
    return VirtualProtect(pAddress, dwSize, PAGE_EXECUTE_READWRITE, &dwOldProtect);
}

VOID FreeStealthMemory(LPVOID pAddress) {
    if (pAddress) {
        VirtualFree(pAddress, 0, MEM_RELEASE);
    }
}
