#ifndef EVASION_H
#define EVASION_H

#include <sys/ptrace.h>

// Evasion functions
int check_debugger();
int check_vm();
int check_sandbox();
void hide_process();
void encrypt_memory();

#endif // EVASION_H