#ifndef EVASION_H
#define EVASION_H

#include <stdint.h>

int evasion_check_debugger();
int evasion_check_vm();
int evasion_hide_files();
int evasion_disable_audit();

#endif // EVASION_H
