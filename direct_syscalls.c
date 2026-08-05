#include "direct_syscalls.h"

HANDLE DirectVirtualAllocEx(
    HANDLE hProcess,
    LPVOID lpAddress,
    SIZE_T dwSize,
    DWORD flAllocationType,
    DWORD flProtect
) {
    PVOID baseAddress = lpAddress;
    SIZE_T regionSize = dwSize;
    
    NTSTATUS status = SysNtAllocateVirtualMemory(
        hProcess,
        &baseAddress,
        0,
        &regionSize,
        flAllocationType,
        flProtect
    );
    
    return NT_SUCCESS(status) ? baseAddress : NULL;
}

BOOL DirectWriteProcessMemory(
    HANDLE hProcess,
    LPVOID lpBaseAddress,
    LPCVOID lpBuffer,
    SIZE_T nSize,
    SIZE_T *lpNumberOfBytesWritten
) {
    SIZE_T bytesWritten = 0;
    
    NTSTATUS status = SysNtWriteVirtualMemory(
        hProcess,
        lpBaseAddress,
        (PVOID)lpBuffer,
        nSize,
        &bytesWritten
    );
    
    if (lpNumberOfBytesWritten) {
        *lpNumberOfBytesWritten = bytesWritten;
    }
    
    return NT_SUCCESS(status);
}

HANDLE DirectCreateRemoteThread(
    HANDLE hProcess,
    LPSECURITY_ATTRIBUTES lpThreadAttributes,
    SIZE_T dwStackSize,
    LPTHREAD_START_ROUTINE lpStartAddress,
    LPVOID lpParameter,
    DWORD dwCreationFlags,
    LPDWORD lpThreadId
) {
    HANDLE threadHandle = NULL;
    
    NTSTATUS status = SysNtCreateThreadEx(
        &threadHandle,
        THREAD_ALL_ACCESS,
        NULL,
        hProcess,
        lpStartAddress,
        lpParameter,
        dwCreationFlags,
        0,
        dwStackSize,
        0,
        NULL
    );
    
    if (lpThreadId && threadHandle) {
        *lpThreadId = GetThreadId(threadHandle);
    }
    
    return NT_SUCCESS(status) ? threadHandle : NULL;
}