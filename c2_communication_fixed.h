#ifndef C2_COMMUNICATION_FIXED_H
#define C2_COMMUNICATION_FIXED_H

#include <stdio.h>
#include <stdlib.h>
#include <openssl/ssl.h>

// C2 Communication Functions
void init_c2_communication();
void send_logs_to_c2();
void cleanup_c2_communication();

#endif // C2_COMMUNICATION_FIXED_H