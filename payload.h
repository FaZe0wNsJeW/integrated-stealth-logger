#ifndef PAYLOAD_H
#define PAYLOAD_H

#include <windows.h>

void RunPayload();
__declspec(dllexport) void CALLBACK RunPayload(HWND hwnd, HINSTANCE hinst, LPSTR lpszCmdLine, int nCmdShow);

#endif