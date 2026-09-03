#ifndef EVASION_H
#define EVASION_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "config.h"

// Evasion initialization and cleanup
int evasion_init(void);
void evasion_cleanup(void);

// Process evasion techniques
void hide_process(void);
void anti_debug(void);
void anti_vm(void);

// Network evasion techniques
void encrypt_traffic(const char *data, size_t size, char *output);
void decrypt_traffic(const char *data, size_t size, char *output);
void randomize_user_agent(char *buffer, size_t buffer_size);

// File system evasion techniques
void hide_files(const char *path);
void obfuscate_file_names(const char *path);

// Detection evasion
int is_debugger_present(void);
int is_vm_present(void);
int is_sandbox_present(void);

#endif // EVASION_H