# VIM3 - A Tiny Vim for Retro Computers

This project is a lightweight, Vim-like text editor designed for 8-bit retro computers, specifically the **Mega65** and **Commodore 64**, with a native build for **Ubuntu** for rapid development and testing.

## Features

*   Modal (Normal/Insert/Command) editing
*   Basic cursor navigation and text manipulation
*   File I/O (Read and Write)
*   Multi-level undo support
*   Platform-specific optimizations for memory and screen handling

## Supported Platforms

The editor is designed to be portable and currently supports the following platforms:

*   **Mega65**: The primary target platform.
*   **Commodore 64**: A port with platform-specific optimizations for the C64's memory and screen limitations.
*   **Ubuntu**: A native Linux build that uses `ncurses`. This is the recommended environment for development, testing, and memory analysis.

## Building and Running

A master shell script, `run.sh`, is provided to handle all build, run, and clean operations.

### Configuration

Before running the script, you may need to edit the `config.prop` file to specify the serial device used for debugging and communication with the Mega65 or C64.

```properties
# Example config.prop
DEBUG_CONSOLE=/dev/ttyUSB0
```

### Usage

*Note: You may need to make the script executable first:* `chmod +x run.sh`

**Build and Run a specific platform:**
*   `./run.sh c64`
*   `./run.sh m65`
*   `./run.sh ubuntu`

**Build all platforms:**
*   `./run.sh all`
    *   This command builds all targets and places the final executables (`vim_c64`, `vim_m65`, `vim_ubuntu`) into the `bin/` directory.

**Memory Analysis (Ubuntu):**
*   `./run.sh ubuntu_mem`
    *   Builds the Ubuntu target in debug mode and runs it under Valgrind to check for memory leaks.

**Clean Up:**
*   `./run.sh clean`
    *   Removes all build directories and binaries from the `bin/` directory.

**Code Formatting:**
*   `./run.sh format`
    *   Automatically formats all C source and header files in the `src/` directory using `clang-format`.

## Command Reference

For a complete list of implemented editor commands, please see the command reference document:

[**doc/commands.txt**](./doc/commands.txt)