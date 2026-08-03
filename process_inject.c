#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Process injection functions

BOOL InjectDllIntoProcess(DWORD dwProcessId, const char* szDllPath) {
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessId);
    if (!hProcess) {
        return FALSE;
    }
    
    LPVOID pRemoteMemory = VirtualAllocEx(hProcess, NULL, strlen(szDllPath) + 1, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!pRemoteMemory) {
        CloseHandle(hProcess);
        return FALSE;
    }
    
    if (!WriteProcessMemory(hProcess, pRemoteMemory, szDllPath, strlen(szDllPath) + 1, NULL)) {
        VirtualFreeEx(hProcess, pRemoteMemory, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return FALSE;
    }
    
    HMODULE hKernel32 = GetModuleHandle("kernel32.dll");
    FARPROC pLoadLibrary = GetProcAddress(hKernel32, "LoadLibraryA");
    
    HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)pLoadLibrary, pRemoteMemory, 0, NULL);
    if (!hThread) {
        VirtualFreeEx(hProcess, pRemoteMemory, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return FALSE;
    }
    
    WaitForSingleObject(hThread, INFINITE);
    
    CloseHandle(hThread);
    VirtualFreeEx(hProcess, pRemoteMemory, 0, MEM_RELEASE);
    CloseHandle(hProcess);
    
    return TRUE;
}
