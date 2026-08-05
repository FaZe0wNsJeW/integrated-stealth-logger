#ifndef SENTINEL_UPGRADE_H
#define SENTINEL_UPGRADE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SENTINEL_VERSION "1.0.0"
#define MAX_BUFFER_SIZE 1024

void sentinel_init();
void sentinel_check_updates();
void sentinel_apply_patch(const char* patch_file);
void sentinel_cleanup();

#endif // SENTINEL_UPGRADE_H