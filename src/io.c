/* File: src/io.c
 *
 * Implementation for input/output functions.
 *
 * Handles writing to files, error reporting, listing licenses, printing descriptions,
 * and searching license texts.
 */

#define _GNU_SOURCE  /* Ensure that strcasestr is declared */

#include "io.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <errno.h>
#include <strings.h>  // For strcasecmp and strcasestr

int write_to_file(const char *filename, const char *content) {
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        return -1;
    }
    if (fputs(content, fp) < 0) {
        fclose(fp);
        return -1;
    }
    fclose(fp);
    return 0;
}

void print_error(const char *message) {
    fprintf(stderr, "\x1b[31m[osla error]: %s\x1b[0m\n", message);
}

void debug_print(const char *message) {
    fprintf(stderr, "[DEBUG]: %s\n", message);
}

void list_license_files(const char *dir_path) {
    DIR *dir = opendir(dir_path);
    if (!dir) {
        print_error("Unable to open licenses directory.");
        return;
    }
    struct dirent *entry;
    printf("Available licenses:\n");
    printf("%-20s %-30s\n", "Short Name", "Aliases / Description");
    printf("-------------------------------------------------------------\n");
    while ((entry = readdir(dir)) != NULL) {
        /* Check if the directory entry is a regular file.
         * Note: Some filesystems may not support d_type. */
        #ifdef DT_REG
        if (entry->d_type == DT_REG) {
        #else
        if (1) { /* Fallback: process all entries */
        #endif
            char *dot = strrchr(entry->d_name, '.');
            if (dot && strcmp(dot, ".txt") == 0) {
                char lic_name[128];
                size_t name_len = dot - entry->d_name;
                if (name_len >= sizeof(lic_name))
                    name_len = sizeof(lic_name) - 1;
                strncpy(lic_name, entry->d_name, name_len);
                lic_name[name_len] = '\0';
                // For simplicity, alias information is hardcoded for known ones
                if (strcasecmp(lic_name, "gpl-3.0") == 0) {
                    printf("%-20s %-30s\n", lic_name, "gpl");
                } else if (strcasecmp(lic_name, "apache-2.0") == 0) {
                    printf("%-20s %-30s\n", lic_name, "apache");
                } else {
                    printf("%-20s %-30s\n", lic_name, "");
                }
            }
        }
    }
    closedir(dir);
}

void print_description(const char *license, const char *desc_dir) {
    char filepath[256];
    snprintf(filepath, sizeof(filepath), "%s/%s.desc", desc_dir, license);
    FILE *fp = fopen(filepath, "r");
    if (!fp) {
        char errmsg[128];
        snprintf(errmsg, sizeof(errmsg), "Description file for '%s' not found.", license);
        print_error(errmsg);
        return;
    }
    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), fp)) {
        printf("%s", buffer);
    }
    fclose(fp);
}

void search_license(const char *keyword, const char *licenses_dir, const char *desc_dir) {
    (void) desc_dir; /* Unused parameter */
    DIR *dir = opendir(licenses_dir);
    if (!dir) {
        print_error("Unable to open licenses directory for searching.");
        return;
    }
    struct dirent *entry;
    printf("Search results for \"%s\":\n", keyword);
    printf("%-20s %-30s\n", "License", "Snippet");
    printf("-------------------------------------------------------------\n");
    while ((entry = readdir(dir)) != NULL) {
        #ifdef DT_REG
        if (entry->d_type == DT_REG) {
        #else
        if (1) {
        #endif
            char *dot = strrchr(entry->d_name, '.');
            if (dot && strcmp(dot, ".txt") == 0) {
                char lic_name[128];
                size_t name_len = dot - entry->d_name;
                if (name_len >= sizeof(lic_name))
                    name_len = sizeof(lic_name) - 1;
                strncpy(lic_name, entry->d_name, name_len);
                lic_name[name_len] = '\0';
                char filepath[256];
                snprintf(filepath, sizeof(filepath), "%s/%s.txt", licenses_dir, lic_name);
                FILE *fp = fopen(filepath, "r");
                if (!fp) continue;
                char snippet[256] = "";
                if (fgets(snippet, sizeof(snippet), fp)) {
                    if (strcasestr(snippet, keyword) || strcasestr(lic_name, keyword)) {
                        printf("%-20s %-30.27s...\n", lic_name, snippet);
                    }
                }
                fclose(fp);
            }
        }
    }
    closedir(dir);
}

