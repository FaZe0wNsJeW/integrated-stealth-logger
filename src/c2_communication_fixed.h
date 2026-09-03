#ifndef C2_COMMUNICATION_FIXED_H
#define C2_COMMUNICATION_FIXED_H

#include "config.h"

int init_c2_communication();
void check_c2_commands();
void send_c2_data(const char* data);

#endif