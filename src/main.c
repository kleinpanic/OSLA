/* File: src/main.c
 *
 * Main entry point for OSLA (Open-source Licence Agreements) command-line tool.
 * This program generates LICENSE files from a local database of license texts,
 * supports configuration, aliases, placeholder expansion, and pretty output.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include "config.h"
#include "license.h"
#include "io.h"
#include "utils.h"
#include "paths.h"
#include "version.h"

#define PROGRAM_NAME "osla"

/* Function prototypes */
static void print_usage(void);
static void print_version(void);
static void list_licenses(bool debug);
static void print_license_description(const char *lic, bool debug);
static void search_licenses(const char *keyword, bool debug);
static void generate_license(const char *lic, bool to_stdout, bool debug);

/* Helper to build a full data path (<datadir>/<subdir>) */
static void build_data_path(const char *subdir, char *buffer, size_t buflen) {
    const char *datadir = get_data_dir();
    snprintf(buffer, buflen, "%s/%s", datadir, subdir);
}

int main(int argc, char *argv[]) {
    bool debug = false;
    bool to_stdout = false;
    bool list = false;
    bool show_help = false;
    bool show_version = false;
    bool default_flag = false;
    bool desc_flag = false;
    bool search_flag = false;
    char *desc_license = NULL;
    char *search_keyword = NULL;
    char *license_arg = NULL;
    
    /* Simple argument parsing */
    for (int i = 1; i < argc; i++) {
        if ((strcmp(argv[i], "-h") == 0) || (strcmp(argv[i], "--help") == 0)) {
            show_help = true;
        } else if ((strcmp(argv[i], "-v") == 0) || (strcmp(argv[i], "--version") == 0)) {
            show_version = true;
        } else if ((strcmp(argv[i], "-d") == 0) || (strcmp(argv[i], "--default") == 0)) {
            default_flag = true;
        } else if ((strcmp(argv[i], "-l") == 0) || (strcmp(argv[i], "--list") == 0)) {
            list = true;
        } else if ((strcmp(argv[i], "-D") == 0) || (strcmp(argv[i], "--description") == 0)) {
            if (i+1 < argc) {
                desc_flag = true;
                desc_license = argv[++i];
            } else {
                print_error("Missing <license> argument for --description flag");
                exit(EXIT_FAILURE);
            }
        } else if (strcmp(argv[i], "--debug") == 0) {
            debug = true;
        } else if (strcmp(argv[i], "--stdout") == 0) {
            to_stdout = true;
        } else if (strcmp(argv[i], "--search") == 0) {
            if (i+1 < argc) {
                search_flag = true;
                search_keyword = argv[++i];
            } else {
                print_error("Missing <keyword> argument for --search flag");
                exit(EXIT_FAILURE);
            }
        } else {
            /* Assume first non-flag argument is license name if not already set */
            if (license_arg == NULL) {
                license_arg = argv[i];
            }
        }
    }
    
    if (show_help) {
        print_usage();
        return EXIT_SUCCESS;
    }

    if (show_version) {
        print_version();
        return EXIT_SUCCESS;
    }

    /* Load configuration (auto-create if missing) */
    Config config;
    if (load_config(&config, debug) != 0) {
        print_error("Failed to load configuration");
        exit(EXIT_FAILURE);
    }
    
    if (default_flag) {
        /* Use default license from config */
        if (strlen(config.default_license) == 0) {
            print_error("No default license set in config.");
            exit(EXIT_FAILURE);
        }
        license_arg = config.default_license;
    }
    
    if (list) {
        list_licenses(debug);
        free_config(&config);
        return EXIT_SUCCESS;
    }
    
    if (desc_flag) {
        print_license_description(desc_license, debug);
        free_config(&config);
        return EXIT_SUCCESS;
    }
    
    if (search_flag) {
        search_licenses(search_keyword, debug);
        free_config(&config);
        return EXIT_SUCCESS;
    }
    
    if (license_arg == NULL) {
        print_error("No license specified. Use -h for help.");
        free_config(&config);
        exit(EXIT_FAILURE);
    }
    
    /* Resolve alias if applicable */
    char resolved_license[64];
    if (resolve_alias(license_arg, resolved_license, sizeof(resolved_license)) != 0) {
        /* If not an alias, copy as-is */
        strncpy(resolved_license, license_arg, sizeof(resolved_license));
        resolved_license[sizeof(resolved_license) - 1] = '\0';
    }
    
    generate_license(resolved_license, to_stdout, debug);
    free_config(&config);
    return EXIT_SUCCESS;
}

/* Prints usage message to stdout */
static void print_usage(void) {
    printf("Usage: %s [options] <license>\n", PROGRAM_NAME);
    printf("Options:\n");
    printf("  -h, --help                 Show this help message.\n");
    printf("  -v, --version              Print program version.\n");
    printf("  -d, --default              Use default license from config file.\n");
    printf("  -l, --list                 List all available licenses.\n");
    printf("  -D, --description <license>    Show formatted description of the license.\n");
    printf("  --debug                    Enable debug output to stderr.\n");
    printf("  --stdout                   Output license to stdout instead of file.\n");
    printf("  --search <keyword>         Search licenses by keyword.\n");
}

/* Prints the version using the version header */
static void print_version(void) {
    printf("%s\n", OSLA_VERSION);
}

/* Lists all available licenses from the global licenses directory */
static void list_licenses(bool debug) {
    char data_path[256];
    build_data_path("licenses", data_path, sizeof(data_path));
    if (debug) {
        char msg[512];  /* increased to 512 bytes */
        snprintf(msg, sizeof(msg), "Listing licenses from directory '%s'", data_path);
        debug_print(msg);
    }
    list_license_files(data_path);
}

/* Prints the description for a given license from the global descriptions directory */
static void print_license_description(const char *lic, bool debug) {
    char data_path[256];
    build_data_path("descriptions", data_path, sizeof(data_path));
    if (debug) {
        char msg[512];  /* increased to 512 bytes */
        snprintf(msg, sizeof(msg), "Printing description for license '%s' from '%s'", lic, data_path);
        debug_print(msg);
    }
    print_description(lic, data_path);
}

/* Searches licenses by keyword in the global licenses directory */
static void search_licenses(const char *keyword, bool debug) {
    char data_path[256];
    build_data_path("licenses", data_path, sizeof(data_path));
    if (debug) {
        char msg[512];  /* increased to 512 bytes */
        snprintf(msg, sizeof(msg), "Searching licenses with keyword '%s' in '%s'", keyword, data_path);
        debug_print(msg);
    }
    search_license(keyword, data_path, NULL);
}

/* Generates the LICENSE file (or outputs to stdout) for the specified license */
static void generate_license(const char *lic, bool to_stdout, bool debug) {
    char data_path[256];
    build_data_path("licenses", data_path, sizeof(data_path));
    if (debug) {
        char msg[512];  /* increased to 512 bytes */
        snprintf(msg, sizeof(msg), "Generating license for '%s' from '%s'", lic, data_path);
        debug_print(msg);
    }
    /* Get license content and perform placeholder replacement from global licenses directory */
    char *content = load_license(lic, data_path);
    if (!content) {
        char errmsg[512];
        snprintf(errmsg, sizeof(errmsg), "[osla error]: license '%s' not found. Try '%s --list' to see available licenses.", lic, PROGRAM_NAME);
        print_error(errmsg);
        exit(EXIT_FAILURE);
    }
    
    /* Replace placeholders <YEAR> and <AUTHOR> */
    Config config;
    if (load_config(&config, debug) != 0) {
        print_error("Failed to load configuration for placeholder expansion");
        free(content);
        exit(EXIT_FAILURE);
    }
    char *filled = replace_placeholders(content, config.year, config.author);
    free(content);
    free_config(&config);

    if (to_stdout) {
        printf("%s", filled);
    } else {
        if (write_to_file("LICENSE", filled) != 0) {
            print_error("Failed to write LICENSE file");
            free(filled);
            exit(EXIT_FAILURE);
        }
        printf("Generated LICENSE file.\n");
    }
    free(filled);
    if (debug) {
        debug_print("License generation completed.");
    }
}

