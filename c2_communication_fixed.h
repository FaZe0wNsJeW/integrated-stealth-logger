#ifndef C2_COMMUNICATION_FIXED_H
#define C2_COMMUNICATION_FIXED_H

int InitC2Communication(const char* server, int port);
void SendC2Message(const char* message);
void CleanupC2Communication();

#endif