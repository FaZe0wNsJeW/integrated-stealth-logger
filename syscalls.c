#include "syscalls.h"
#include <stdint.h>

// SysWhispers4 generated syscall stubs
__declspec(naked) NTSTATUS SysNtAllocateVirtualMemory(
    HANDLE ProcessHandle,
    PVOID *BaseAddress,
    ULONG ZeroBits,
    PSIZE_T RegionSize,
    ULONG AllocationType,
    ULONG Protect
) {
    __asm {
        mov r10, rcx
        mov eax, 0x18
        syscall
        ret
    }
}

__declspec(naked) NTSTATUS SysNtWriteVirtualMemory(
    HANDLE ProcessHandle,
    PVOID BaseAddress,
    PVOID Buffer,
    SIZE_T BufferSize,
    PSIZE_T NumberOfBytesWritten
) {
    __asm {
        mov r10, rcx
        mov eax, 0x3A
        syscall
        ret
    }
}

__declspec(naked) NTSTATUS SysNtCreateThreadEx(
    PHANDLE ThreadHandle,
    ACCESS_MASK DesiredAccess,
    POBJECT_ATTRIBUTES ObjectAttributes,
    HANDLE ProcessHandle,
    PVOID StartRoutine,
    PVOID Argument,
    ULONG CreateFlags,
    SIZE_T ZeroBits,
    SIZE_T StackSize,
    SIZE_T MaximumStackSize,
    PVOID AttributeList
) {
    __asm {
        mov r10, rcx
        mov eax, 0xCE
        syscall
        ret
    }
}