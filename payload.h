#ifndef PAYLOAD_H
#define PAYLOAD_H

#include <stdint.h>

int payload_init();
void payload_cleanup();
int payload_log_key(int key);
int payload_log_string(const char *str);
const char *payload_get_version();

#endif // PAYLOAD_H
