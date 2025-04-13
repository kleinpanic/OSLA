/* File: src/paths.h
 *
 * Defines the global data path for OSLA, and provides a helper to get the effective data directory.
 * At runtime, the OSLA_DATADIR environment variable is checked; if not set, the compiled default is used.
 */

#ifndef PATHS_H
#define PATHS_H

#ifndef OSLA_DATA_DIR
#define OSLA_DATA_DIR "/usr/local/share/osla"
#endif

/* Returns the data directory to use – OSLA_DATADIR environment variable if set, else OSLA_DATA_DIR */
static inline const char *get_data_dir(void) {
    const char *env = getenv("OSLA_DATADIR");
    return (env && env[0] != '\0') ? env : OSLA_DATA_DIR;
}

#endif /* PATHS_H */

