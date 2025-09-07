# VIM3 C Coding Style Guide

This document outlines the coding conventions and style used in the VIM3 project. Following these guidelines ensures that the codebase remains consistent, readable, and maintainable.

## Naming Conventions

### Functions

-   Functions are named using `camelCase` (e.g., `setEditMode`, `allocLine`).
-   Functions that are part of the **Platform Abstraction Layer** are prefixed with `pl` (e.g., `plSetCursor`, `plGetKey`).

### Variables

-   Variable names are prefixed according to their type to provide Hungarian-style notation. Prefixes are combined for complex types (e.g., `p2z` for `char **`).

| Prefix | Type                               | Example                     |
| :----- | :--------------------------------- | :-------------------------- |
| `c`    | Character                          | `char cKeyPressed;`         |
| `z`    | Zero-terminated string (`char[]`)  | `char zFilename[81];`       |
| `i`    | Integer (`int`, `uint16_t`, etc.)  | `int iCounter;`             |
| `l`    | Long (`long`)                      | `long lTimestamp;`          |
| `p`    | Pointer                            | `tsState *psState;`         |
| `p2`   | Double Pointer                     | `char **p2zText;`           |
| `e`    | Enum variable                      | `EditMode eCurrentMode;`    |

-   After the prefix, the variable name uses `PascalCase` for struct members (e.g., `psState->iLineY`) and `camelCase` for local variables (e.g., `int iLineIndex`).

### Type Definitions (structs, enums)

-   Structs defined with `typedef` are prefixed with `ts` (for "Typedef Struct") and use `PascalCase` (e.g., `tsState`, `tsXYPos`).
-   Enums defined with `typedef` are prefixed with `e` and use `PascalCase` (e.g., `EditMode`, `eVimKeyCode`).

### Constants and Macros

-   Constants and macros are defined using `UPPER_CASE_SNAKE_CASE` (e.g., `MAX_LINE_LENGTH`, `DEBUG_ON`).

## Formatting

### Braces

-   The project follows the **K&R (Kernighan & Ritchie) style**. The opening brace for a control statement (`if`, `switch`, `while`, etc.) or function definition is placed on the same line.

    ```c
    // Correct
    void myFunction(int arg) {
        if (arg > 0) {
            // ...
        }
    }
    ```

### Indentation

-   Indentation is done with **4 spaces**. Tabs should not be used.

### Spacing

-   Use a single space around binary operators (`=`, `+`, `-`, `*`, `/`, `==`, etc.).
-   Do not place a space between a function name and its opening parenthesis.
-   Place a space after commas in argument lists.

    ```c
    // Correct
    int result = x + y;
    myFunction(arg1, arg2);
    ```

## File Structure

### Header Guards

-   Header files must be protected from multiple inclusions using `#ifndef`/`#define`/`#endif` guards. The guard name should be based on the filename in uppercase with underscores.

    ```c
    #ifndef MY_HEADER_H
    #define MY_HEADER_H

    // ... header content ...

    #endif // MY_HEADER_H
    ```

### Includes

-   The order of includes should be:
    1.  Standard library headers (`<string.h>`, `<stdio.h>`)
    2.  Project-specific headers (`"platform.h"`, `"state.h"`)

## Platform Specifics

-   Platform-specific code must be isolated within `#if defined(...)` blocks.
-   Whenever possible, abstract platform-specific constants or functions into the platform header files (`src/platform/<name>/platform_<name>.h`) and the `platform.h` interface to avoid `#ifdef` blocks in the core application logic.
