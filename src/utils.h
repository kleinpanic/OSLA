/* File: src/utils.h
 *
 * Header for utility functions.
 *
 * Provides commonly used helper functions for string manipulation, directory operations, and file checks.
 */

#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>
#include <string.h>

/* Trims newline characters from the end of a string */
static inline void trim_newline(char *str) {
    size_t len = strlen(str);
    while(len > 0 && (str[len-1] == '\n' || str[len-1] == '\r')) {
        str[len-1] = '\0';
        len--;
    }
}

/* Ensures that the specified directory exists.
 * Returns 0 if the directory exists or was created, non-zero on error.
 */
int ensure_directory_exists(const char *path);

#endif /* UTILS_H */

