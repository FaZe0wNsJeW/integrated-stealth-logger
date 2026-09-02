#ifndef C2_COMMUNICATION_FIXED_H
#define C2_COMMUNICATION_FIXED_H

#include "payload_config.h"

// C2 communication functions
int init_c2_communication();
int send_c2_data(const char *data, size_t len);
int receive_c2_data(char *buffer, size_t len);
void cleanup_c2_communication();

#endif // C2_COMMUNICATION_FIXED_H