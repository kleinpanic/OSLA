/* File: src/license.c
 *
 * Implementation for license related functionalities.
 *
 * Responsible for reading license files, performing text placeholder replacement,
 * and mapping alias names to full license names.
 */

#define _GNU_SOURCE  /* Ensure proper feature test macros for functions like strcasecmp */

#include "license.h"
#include "utils.h"
#include "io.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>

/* Mapping for license aliases */
typedef struct {
    const char *alias;
    const char *full;
} AliasMapping;

static const AliasMapping alias_map[] = {
    {"gpl", "gpl-3.0"},
    {"apache", "apache-2.0"},
    {NULL, NULL}
};

char *load_license(const char *license, const char *licenses_dir) {
    char filepath[256];
    snprintf(filepath, sizeof(filepath), "%s/%s.txt", licenses_dir, license);
    
    FILE *fp = fopen(filepath, "r");
    if (!fp) {
        return NULL;
    }
    
    // Get file size
    if (fseek(fp, 0, SEEK_END) != 0) {
        fclose(fp);
        return NULL;
    }
    long filesize = ftell(fp);
    if (filesize < 0) {
        fclose(fp);
        return NULL;
    }
    rewind(fp);
    
    char *buffer = (char *)malloc(filesize + 1);
    if (!buffer) {
        fclose(fp);
        return NULL;
    }
    size_t read_size = fread(buffer, 1, filesize, fp);
    if (read_size != (size_t)filesize) {
        free(buffer);
        fclose(fp);
        return NULL;
    }
    buffer[filesize] = '\0';
    fclose(fp);
    return buffer;
}

char *replace_placeholders(const char *content, const char *year, const char *author) {
    size_t buf_size = strlen(content) + 256;
    char *result = (char *)malloc(buf_size);
    if (!result) return NULL;
    result[0] = '\0';
    
    const char *p = content;
    while (*p) {
        if (strncmp(p, "<YEAR>", 6) == 0) {
            strncat(result, year, buf_size - strlen(result) - 1);
            p += 6;
        } else if (strncmp(p, "<AUTHOR>", 8) == 0) {
            strncat(result, author, buf_size - strlen(result) - 1);
            p += 8;
        } else {
            size_t len = strlen(result);
            result[len] = *p;
            result[len + 1] = '\0';
            p++;
        }
    }
    return result;
}

int resolve_alias(const char *input, char *output, size_t out_size) {
    for (int i = 0; alias_map[i].alias != NULL; i++) {
        if (strcasecmp(input, alias_map[i].alias) == 0) {
            strncpy(output, alias_map[i].full, out_size);
            output[out_size - 1] = '\0';
            return 0;
        }
    }
    return -1;
}

