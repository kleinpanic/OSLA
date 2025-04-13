/* File: src/config.c
 *
 * Implementation for configuration file handling.
 *
 * Reads configuration from ~/.config/OSLA/osla.conf, auto-creates config if missing,
 * and populates the Config structure.
 */

#include "config.h"
#include "io.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define CONFIG_DIR "~/.config/OSLA"
#define CONFIG_PATH "~/.config/OSLA/osla.conf"
#define DEFAULT_AUTHOR "Your Name"
#define DEFAULT_YEAR "2025"
#define DEFAULT_LICENSE "mit"

/* Helper function to expand home directory shorthand ~ */
static void expand_home(char *path, size_t size) {
    const char *home = getenv("HOME");
    if (!home) {
        return;
    }
    if (path[0] == '~') {
        char temp[256];
        snprintf(temp, sizeof(temp), "%s%s", home, path + 1);
        strncpy(path, temp, size);
        path[size - 1] = '\0';
    }
}

int load_config(Config *config, int debug) {
    /* Set defaults first */
    strncpy(config->author, DEFAULT_AUTHOR, sizeof(config->author));
    strncpy(config->year, DEFAULT_YEAR, sizeof(config->year));
    strncpy(config->default_license, DEFAULT_LICENSE, sizeof(config->default_license));
    
    char config_path[256] = CONFIG_PATH;
    expand_home(config_path, sizeof(config_path));
    
    FILE *fp = fopen(config_path, "r");
    if (!fp) {
        /* Config file does not exist. Create one with defaults. */
        char config_dir[256] = CONFIG_DIR;
        expand_home(config_dir, sizeof(config_dir));
        if (ensure_directory_exists(config_dir) != 0) {
            print_error("Failed to create config directory");
            return -1;
        }
        fp = fopen(config_path, "w");
        if (!fp) {
            print_error("Failed to create default configuration file");
            return -1;
        }
        fprintf(fp, "author=%s\nyear=%s\ndefault_license=%s\n", DEFAULT_AUTHOR, DEFAULT_YEAR, DEFAULT_LICENSE);
        fclose(fp);
        if (debug) {
            debug_print("Default configuration file created");
        }
        return 0;
    }
    
    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        trim_newline(line);
        char *eq = strchr(line, '=');
        if (!eq) continue;
        *eq = '\0';
        char *key = line;
        char *value = eq + 1;
        if (strcmp(key, "author") == 0) {
            strncpy(config->author, value, sizeof(config->author));
            config->author[sizeof(config->author)-1] = '\0';
        } else if (strcmp(key, "year") == 0) {
            strncpy(config->year, value, sizeof(config->year));
            config->year[sizeof(config->year)-1] = '\0';
        } else if (strcmp(key, "default_license") == 0) {
            strncpy(config->default_license, value, sizeof(config->default_license));
            config->default_license[sizeof(config->default_license)-1] = '\0';
        }
    }
    fclose(fp);
    if (debug) {
        debug_print("Configuration loaded successfully");
    }
    return 0;
}

void free_config(Config *config) {
    (void)config;
}

