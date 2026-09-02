#ifndef PAYLOAD_H
#define PAYLOAD_H

#include <pthread.h>

// Payload functions
void init_payload();
void stop_payload();
int is_payload_running();

// Thread main functions
void* keylogger_main(void* arg);
void* screenshot_main(void* arg);
void* c2_communication_main(void* arg);

#endif // PAYLOAD_H