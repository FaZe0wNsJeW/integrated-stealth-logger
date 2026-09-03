#ifndef EVASION_H
#define EVASION_H

#include <stdio.h>
#include <stdlib.h>

// Evasion Functions
void init_evasion();
void check_for_debuggers();
void check_for_virtual_machines();
void enable_fileless_mode();

#endif // EVASION_H