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
#include <stdbool.h>

const AliasMapping alias_map[] = {
//  {"alias", "name"},    
    {"gpl", "gpl-3.0"},
    {"apache", "apache-2.0"},
    {"agpl", "gnu-agplv-3.0"},
    {"lgpl", "gnu-lgplv-3.0"},
    {"mpl", "mpl-2.0"},
    {"bsd-2", "bsd-2-clause"},
    {"bsd-3", "bsd-3-clause"},
    {"mit", "mit"},
    {"ul", "unlicense"},
    {"cern-ohl-w", "cern-ohl-w-2.0"},
    {"cern-ohl-p", "cern-ohl-p-2.0"},
    {"cern-ohl-s", "cern-ohl-s-2.0"},
    {"sil-ofl", "sil-ofl-1.1"},
    {"wtfpl", "wtfpl-2.0"},
    {"cc0", "cc0-1.0"},
    {"cc-by-sa", "cc-by-sa-4.0"},
    {"cc-by", "cc-by-4.0"},
    {"boost", "boost"},
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
    typedef struct { const char *token; const char *value; } TokenMap;
    const TokenMap tokens[] = {
        { "<YEAR>",              year },
        { "[yyyy]",              year },
        { "[year]",              year },
        { "(c) [year]",          NULL },   // keep "(c)" literal, replace only [year]
        { "[year]",              year },
        { "<AUTHOR>",            author },
        { "[name of copyright owner]", author },
        { "[fullname]",          author },
        { "<COPYRIGHT HOLDER>",  author },
        { "[copyright holder]",  author },
        { NULL,                  NULL }
    };
    // Estimate output size generously
    size_t buf_size = strlen(content) + 512;
    char *result = malloc(buf_size);
    if (!result) return NULL;
    result[0] = '\0';

    const char *p = content;
    while (*p) {
        bool matched = false;
        for (int i = 0; tokens[i].token; i++) {
            size_t tlen = strlen(tokens[i].token);
            if (strncmp(p, tokens[i].token, tlen) == 0) {
                if (tokens[i].value) {
                    strncat(result, tokens[i].value, buf_size - strlen(result) - 1);
                } else {
                    // Special case "(c) [year]": output "(c) " then year
                    if (strcmp(tokens[i].token, "(c) [year]") == 0) {
                        strncat(result, "(c) ", buf_size - strlen(result) - 1);
                        strncat(result, year, buf_size - strlen(result) - 1);
                    }
                }
                p += tlen;
                matched = true;
                break;
            }
        }
        if (!matched) {
            // Copy one character
            size_t len = strlen(result);
            result[len] = *p++;
            result[len + 1] = '\0';
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

