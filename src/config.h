/* File: src/config.h
 *
 * Header for configuration file handling.
 *
 * Provides functionalities to load and free configuration settings from a configuration file.
 */

#ifndef CONFIG_H
#define CONFIG_H

typedef struct {
    char author[128];
    char year[16];
    char default_license[64];
} Config;

/* Loads the configuration from ~/.config/osla/osla.conf.
 * Automatically creates a default configuration if the file does not exist.
 * Returns 0 on success, non-zero on failure.
 */
int load_config(Config *config, int debug);

/* Frees resources allocated for the configuration.
 * Currently a stub since Config uses static allocations.
 */
void free_config(Config *config);

#endif /* CONFIG_H */

