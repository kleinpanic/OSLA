/* File: src/utils.c
 *
 * Implementation of utility functions.
 *
 * Includes functions for trimming strings and ensuring directory existence.
 */

#include "utils.h"
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

/* A simple implementation to ensure a directory exists.
 * Uses mkdir on POSIX systems.
 */
int ensure_directory_exists(const char *path) {
    struct stat st;
    if (stat(path, &st) == 0) {
        if (S_ISDIR(st.st_mode)) {
            return 0;
        } else {
            return -1;
        }
    }
    // Create the directory with permissions 0755
    if (mkdir(path, 0755) != 0) {
        return -1;
    }
    return 0;
}

