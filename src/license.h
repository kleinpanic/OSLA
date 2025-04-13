/* File: src/license.h
 *
 * Header for license-related functions.
 *
 * Provides functions to load license texts, replace placeholders, and list available licenses.
 */

#ifndef LICENSE_H
#define LICENSE_H

#include <stddef.h>  /* Added to define size_t */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Loads the license text for the given license short name from the specified directory.
 * Returns a dynamically allocated string containing the license text,
 * or NULL if the file is not found. Caller must free the returned string.
 */
char *load_license(const char *license, const char *licenses_dir);

/* Replaces placeholders <YEAR> and <AUTHOR> in the given content with provided values.
 * Returns a new dynamically allocated string with replacements.
 */
char *replace_placeholders(const char *content, const char *year, const char *author);

/* Resolves a license alias to its full name. Returns 0 if resolved, non-zero otherwise.
 * If the alias is not found, license is unchanged.
 */
int resolve_alias(const char *input, char *output, size_t out_size);

#endif /* LICENSE_H */

