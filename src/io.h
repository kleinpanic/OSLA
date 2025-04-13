/* File: src/io.h
 *
 * Header for input/output related functions.
 *
 * Provides functions for file writing, formatted output, and error messaging.
 */

#ifndef IO_H
#define IO_H

/* Writes the content to a file with the given filename.
 * Returns 0 on success, non-zero on failure.
 */
int write_to_file(const char *filename, const char *content);

/* Prints an error message to stderr with a consistent format. */
void print_error(const char *message);

/* Debug printing to stderr if enabled. */
void debug_print(const char *message);

/* Lists license files in the given directory in a tabulated format. */
void list_license_files(const char *dir_path);

/* Prints a formatted description for a given license from a descriptions directory. */
void print_description(const char *license, const char *desc_dir);

/* Searches licenses by keyword in the specified directories. */
void search_license(const char *keyword, const char *licenses_dir, const char *desc_dir);

#endif /* IO_H */

