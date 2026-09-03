#ifndef C2_COMMUNICATION_FIXED_H
#define C2_COMMUNICATION_FIXED_H

#include <openssl/ssl.h>

// C2 Communication functions
void init_c2_communication();
void send_data(const char* data, size_t length);
void receive_data(char* buffer, size_t length);
void send_heartbeat();
void close_c2_communication();

#endif // C2_COMMUNICATION_FIXED_H