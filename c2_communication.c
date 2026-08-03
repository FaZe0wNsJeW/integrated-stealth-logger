#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

// C2 communication functions

SOCKET InitializeC2Connection(const char* szServerIP, int nPort) {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        return INVALID_SOCKET;
    }
    
    SOCKET hSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (hSocket == INVALID_SOCKET) {
        WSACleanup();
        return INVALID_SOCKET;
    }
    
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(nPort);
    inet_pton(AF_INET, szServerIP, &serverAddr.sin_addr);
    
    if (connect(hSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        closesocket(hSocket);
        WSACleanup();
        return INVALID_SOCKET;
    }
    
    return hSocket;
}

BOOL SendC2Data(SOCKET hSocket, const char* szData, DWORD dwSize) {
    if (hSocket == INVALID_SOCKET) {
        return FALSE;
    }
    
    int nBytesSent = send(hSocket, szData, dwSize, 0);
    return (nBytesSent == dwSize);
}

DWORD ReceiveC2Data(SOCKET hSocket, char* szBuffer, DWORD dwBufferSize) {
    if (hSocket == INVALID_SOCKET) {
        return 0;
    }
    
    int nBytesReceived = recv(hSocket, szBuffer, dwBufferSize, 0);
    return (nBytesReceived > 0) ? nBytesReceived : 0;
}

VOID CloseC2Connection(SOCKET hSocket) {
    if (hSocket != INVALID_SOCKET) {
        closesocket(hSocket);
    }
    WSACleanup();
}
