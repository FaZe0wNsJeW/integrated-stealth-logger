#ifndef C2_COMMUNICATION_FIXED_H
#define C2_COMMUNICATION_FIXED_H

#include <stdint.h>

int c2_init();
int c2_send_data(const char *data, size_t length);
int c2_receive_data(char *buffer, size_t max_length);
void c2_cleanup();

#endif // C2_COMMUNICATION_FIXED_H
