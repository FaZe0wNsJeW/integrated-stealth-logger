#ifndef ANTI_DEBUG_H
#define ANTI_DEBUG_H

#include <windows.h>
#include <intrin.h>

// Anti-debugging functions
void AntiDebugChecks();
BOOL IsDebuggerPresent();
BOOL CheckRemoteDebuggerPresent(HANDLE hProcess, PBOOL pbDebuggerPresent);
BOOL IsBlacklistedProcess();

// String obfuscation
char* XorString(char* str, size_t len);

// Macros
#define CHECK_DEBUGGER() if (IsDebuggerPresent() || CheckRemoteDebuggerPresent(GetCurrentProcess(), NULL)) ExitProcess(0)
#define CHECK_BREAKPOINT() __try { __debugbreak(); } __except(EXCEPTION_EXECUTE_HANDLER) { ExitProcess(0); }
#define CHECK_TIMING() LARGE_INTEGER start, end, freq; QueryPerformanceFrequency(&freq); QueryPerformanceCounter(&start); Sleep(100); QueryPerformanceCounter(&end); if ((end.QuadPart - start.QuadPart) < freq.QuadPart / 10) ExitProcess(0)
#define OBFUSCATE(str) XorString(str, sizeof(str)-1)

#endif