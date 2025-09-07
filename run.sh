#!/bin/bash

# Master script for building, running, and cleaning the VIM3 project.

# --- Configuration ---
# Source the configuration file for the DEBUG_CONSOLE variable
if [ -f "config.prop" ]; then
    source config.prop
else
    echo "Configuration file config.prop not found."
    # Set a default to allow clean commands to run without a config file
    DEBUG_CONSOLE=/dev/ttyUSB0
fi

# --- Functions ---

build_c64() {
    echo ""
    echo "--- Building C64 ---"
    rm -rf cmake-build-c64
    cmake -B cmake-build-c64 -S . -DPLATFORM=c64 && \
    cmake --build cmake-build-c64 && \
    cmake --install cmake-build-c64
}

build_m65() {
    echo ""
    echo "--- Building Mega65 ---"
    rm -rf cmake-build-m65
    cmake -B cmake-build-m65 -S . -DPLATFORM=mega65 && \
    cmake --build cmake-build-m65 && \
    cmake --install cmake-build-m65
}

build_ubuntu() {
    echo ""
    echo "--- Building Ubuntu ---"
    rm -rf cmake-build-ubuntu
    cmake -B cmake-build-ubuntu -S . -DPLATFORM=ubuntu && \
    cmake --build cmake-build-ubuntu && \
    cmake --install cmake-build-ubuntu
}

run_ubuntu_mem() {
    echo ""
    echo "--- Building for Ubuntu (Debug) and Running with Valgrind ---"
    rm -rf cmake-build-ubuntu-memcheck
    cmake -B cmake-build-ubuntu-memcheck -S . -DPLATFORM=ubuntu -DCMAKE_BUILD_TYPE=Debug -DENABLE_NCURSES_MEM_DEBUG=ON && \
    cmake --build cmake-build-ubuntu-memcheck && \
    valgrind --leak-check=full --track-origins=yes ./cmake-build-ubuntu-memcheck/vim_ubuntu
}

build_all() {
    echo "--- Building all platforms and installing to bin/ ---"
    
    build_ubuntu
    build_c64
    build_m65

    echo ""
    echo "--- All builds complete. Executables are in the bin/ directory: ---"
    ls -l bin/
}


dist_clean() {
    echo "--- Performing a deep clean ---"
    # Use a temporary build directory and the dedicated cleanup script
    if [ ! -d "cmake-build-temp" ]; then
        mkdir cmake-build-temp
    fi
    cmake -B cmake-build-temp -S ./cmake && \
    cmake --build cmake-build-temp --target distclean
    # Remove the temporary directory
    rm -rf cmake-build-temp
    echo "--- Clean complete. ---"
}

format_code() {
    echo "--- Formatting C source code ---"
    if ! command -v clang-format &> /dev/null
    then
        echo "ERROR: clang-format is not installed or not in your PATH."
        echo "Please install it to use the format command."
        echo "On Debian/Ubuntu, you can use: sudo apt-get install clang-format"
        exit 1
    fi
    find src -name "*.c" -o -name "*.h" | xargs clang-format -i
    echo "--- Formatting complete. ---"
}

# --- Main Logic ---

if [ -z "$1" ]; then
    echo "Usage: ./run.sh [c64|m65|ubuntu|ubuntu_mem|all|clean|format]"
    exit 1
fi

case "$1" in
    c64)
        build_c64 && m65 -l $DEBUG_CONSOLE -4 -r bin/vim_c64
        ;; 
    m65)
        build_m65 && m65 -l $DEBUG_CONSOLE -F -r bin/vim_m65
        ;; 
    ubuntu)
        build_ubuntu && ./bin/vim_ubuntu
        ;; 
    ubuntu_mem)
        run_ubuntu_mem
        ;; 
    all)
        build_all
        ;; 
    clean)
        dist_clean
        ;; 
    format)
        format_code
        ;; 
    *)
        echo "Invalid command: $1"
        echo "Usage: ./run.sh [c64|m65|ubuntu|ubuntu_mem|all|clean|format]"
        exit 1
        ;; 
esac

