# OSLA — Open-source Licence Agreements

OSLA is a command-line tool for generating LICENSE files from a local, offline database of license texts. It supports a variety of popular open-source licenses—including non-software licenses—and provides features such as placeholder replacement, interactive configuration, alias resolution, and formatted output.

## Features

- **Generate LICENSE Files:**  
  Easily create a LICENSE file for a specified license, with automatic placeholder replacements (e.g. `<YEAR>`, `<COPYRIGHT HOLDER>`).

- **Configuration Management:**  
  Reads user configuration from `~/.config/OSLA/osla.conf` (auto-creates the file with sensible defaults if missing).

- **Global and Local Operation:**  
  - *Global Install:* Installs the binary to `/usr/local/bin`, license data to `/usr/local/share/osla`, and defaults the configuration to `~/.config/OSLA/osla.conf`.
  - *Local Development:* Use the provided Makefile target (`local-env`) to export the OSLA_DATADIR variable for testing without a global install.

- **Additional Options:**  
  - List available licenses.
  - Display formatted license descriptions.
  - Search licenses by keyword.
  - Support for aliases (customizable in the source code).

## Project Structure

```plaintext
osla/
├── src/                     # C source code
│   ├── config.c & config.h  # Configuration file handling
│   ├── license.c & license.h# License file loading and placeholder replacement
│   ├── io.c & io.h          # Input/output functions (file reading, error reporting)
│   ├── main.c               # Main program entry point
│   ├── paths.h              # Data directory path management (OSLA_DATADIR)
│   └── version.h            # Contains the version string
├── licenses/                # Directory containing all license text files (*.txt)
├── descriptions/            # Directory containing formatted license descriptions (*.desc)
├── config/                  # Example configuration file (osla.conf.example)
├── Makefile                 # Build, install, and environment setup targets
└── README.md                # This file
```

## Usage

### Basic Invocation

Generate a LICENSE file for a specified license (by short name, e.g., `mit`):

```bash
osla mit
```

This creates a file named `LICENSE` in the current directory with the full text of the specified license (with `<YEAR>` and `<COPYRIGHT HOLDER>` placeholders replaced).

### Options

- `-h, --help`  
  Show help and usage information.

- `-v, --version`  
  Print the OSLA version.

- `-d, --default`  
  Use the default license from the configuration file.

- `-l, --list`  
  List all available licenses along with any aliases.

- `-D, --description <license>`  
  Show a formatted description for the specified license.

- `--debug`  
  Enable debug output for detailed execution steps.

- `--stdout`  
  Output the generated license to stdout instead of writing to a file.

- `--search <keyword>`  
  Search for licenses by a keyword in the license names or descriptions.

### Example Commands

- **List Licenses:**
  ```bash
  osla --list
  ```

- **Generate a LICENSE File Using the Default License:**
  ```bash
  osla --default
  ```

- **Output License Text to STDOUT:**
  ```bash
  osla mit --stdout
  ```

- **Search for "permissive" Licenses:**
  ```bash
  osla --search permissive
  ```

## Installation

### Global Installation

To install OSLA globally (so that it is available from anywhere), run:
  
```bash
make clean all
sudo make install
```

- **Binary:** Installed to `/usr/local/bin/osla`.
- **License Data:** Installed to `/usr/local/share/osla` (licenses in the `licenses/` directory and descriptions in the `descriptions/` directory).
- **Configuration:** The default configuration file is copied to `~/.config/OSLA/osla.conf`.

### Local Development

If you prefer to test OSLA in your development directory without a global install, set the environment variable `OSLA_DATADIR` to the project root. Use the provided `local-env` target:

```bash
source <(make local-env)
./osla --list
```

This sets `OSLA_DATADIR` to your current working directory so that OSLA uses your local `licenses/` and `descriptions/` directories.

## Adding New Licenses

To add new licenses:

1. **License Text:**  
   Add a new `.txt` file to the `licenses/` directory. Use `<YEAR>` and `<COPYRIGHT HOLDER>` placeholders if you want them auto-replaced.

2. **License Description:**  
   Add a corresponding `.desc` file to the `descriptions/` directory with a brief description of the license.

3. **Aliases (Optional):**  
   If you’d like to add an alias, update the alias mapping in `src/license.c`.

## License

OSLA itself is provided under a permissive license. Refer to the individual license file in the repository if needed.

## Contributing

Contributions, bug fixes, and suggestions are welcome. Please feel free to submit pull requests or open issues on the project repository.

## Acknowledgments

This project follows the UNIX philosophy of small, modular, and composable tools.
This project makes use of online resources to retrieve the license agreements including:
* https://opensource.org/license/bsd-2-clause
* https://opensource.org/license/bsd-3-clause
* https://choosealicense.com/licenses/
* https://choosealicense.com/non-software/
```
