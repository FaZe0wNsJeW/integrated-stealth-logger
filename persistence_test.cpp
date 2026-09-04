// persistence_test.cpp
#include <windows.h>

// This is the function your RunModule() will look for and execute.
extern "C" __declspec(dllexport) void RunPayload() {
    // For testing, we'll just show a message box.
    // In a real module, you would call your persistence functions here.
    MessageBoxA(NULL, "Persistence Module Executed Successfully!", "Test", MB_OK);
}