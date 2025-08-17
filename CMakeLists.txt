cmake_minimum_required(VERSION 3.18)

# --- Platform Selection ---
set(SUPPORTED_PLATFORMS "mega65" "c64" "ubuntu")
if(NOT PLATFORM)
    message(FATAL_ERROR "PLATFORM not set. Please specify a platform with -DPLATFORM=<platform>. Supported platforms are: ${SUPPORTED_PLATFORMS}")
endif()
list(FIND SUPPORTED_PLATFORMS "${PLATFORM}" IS_SUPPORTED)
if(${IS_SUPPORTED} EQUAL -1)
    message(FATAL_ERROR "Unsupported platform: ${PLATFORM}. Supported platforms are: ${SUPPORTED_PLATFORMS}")
endif()
message(STATUS "Building for platform: ${PLATFORM}")

# --- Platform Specific Setup ---
if (${PLATFORM} STREQUAL "mega65")
    set(__MEGA65__ 65)
    # @buildIssue1 list(APPEND CORE_SOURCES src/core/lib/snprintf.c)
    set(CMAKE_C_COMPILER /home/duck/m65/c/llvm-mos/bin/mos-mega65-clang)
    set(CMAKE_PREFIX_PATH /home/duck/m65/c/llvm-mos)
    find_package(llvm-mos-sdk REQUIRED)
    set(LLVM_MOS_PLATFORM mega65)
elseif (${PLATFORM} STREQUAL "c64")
    set(CMAKE_C_COMPILER /home/duck/m65/c/llvm-mos/bin/mos-c64-clang)
    set(CMAKE_PREFIX_PATH /home/duck/m65/c/llvm-mos)
    find_package(llvm-mos-sdk REQUIRED)
    set(LLVM_MOS_PLATFORM c64)
elseif (${PLATFORM} STREQUAL "ubuntu")
    set(CMAKE_C_COMPILER /usr/bin/cc)
    set(CMAKE_CXX_COMPILER /usr/bin/c++)
endif()

# --- Basic Project Setup ---
project(vim)
option(ENABLE_DEBUG "Enable debug messages for the main vim executable" ON)

# --- Common C Flags ---
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-signed-char")

# --- Core Source Files (Platform Independent) ---
set(CORE_SOURCES
    src/core/vim.c
    src/core/cmd.c
    src/core/editor.c
    src/core/render.c
    src/core/line.c
    src/core/lib/itostr.c
)

# --- Add packages and sources based on platform ---
if (${PLATFORM} STREQUAL "mega65")
    include(FetchContent)
    FetchContent_Declare(CPM GIT_REPOSITORY https://github.com/cpm-cmake/CPM.cmake GIT_TAG v0.42.0)
    FetchContent_MakeAvailable(CPM)
    CPMAddPackage(NAME mega65libc GITHUB_REPOSITORY CTalkobt/mega65-libc GIT_TAG development)
    include_directories(${mega65libc_SOURCE_DIR}/include)

    list(APPEND PLATFORM_SOURCES
        src/platform/mega65/platform_mega65.c
        src/platform/mega65/screen.c
        src/platform/mega65/kbd.c
        src/platform/mega65/kernal.c
    )
    set(PLATFORM_LIBRARIES mega65libc)
    set(PLATFORM_LINK_FLAGS "-Wl,-z,stack-size=8192" "-Wl,-Map=vim.map")

elseif (${PLATFORM} STREQUAL "c64")
    list(APPEND PLATFORM_SOURCES src/platform/c64/platform_c64.c)
    # Add C64 libraries here

elseif (${PLATFORM} STREQUAL "ubuntu")
    find_package(Curses REQUIRED)
    list(APPEND PLATFORM_SOURCES src/platform/ubuntu/platform_ubuntu.c)
    set(PLATFORM_LIBRARIES ${CURSES_LIBRARIES})
endif()


# --- Main Executable Target ---
add_executable(vim ${CORE_SOURCES} ${PLATFORM_SOURCES})
target_include_directories(vim PRIVATE src/core src/platform)
target_link_libraries(vim PRIVATE ${PLATFORM_LIBRARIES})
target_link_options(vim PRIVATE ${PLATFORM_LINK_FLAGS})

if (${PLATFORM} STREQUAL "mega65")
    target_compile_definitions(vim PRIVATE __MEGA65__)
    target_include_directories(vim PRIVATE ${mega65libc_SOURCE_DIR}/include)
endif()

if(ENABLE_DEBUG)
    target_compile_definitions(vim PRIVATE DEBUG_ON)
endif()

# --- Compiler Warnings ---
target_compile_options(vim PRIVATE
    -Wshadow
    -Wcast-align
    -Wunused
    -Wnull-dereference
    -Wformat=2
    -Wimplicit-fallthrough
    -Wno-incompatible-pointer-types-discards-qualifiers
    -Wno-sign-conversion
    -Wno-int-conversion
    -Wno-pointer-sign
    -fno-signed-char
)

if (${PLATFORM} STREQUAL "mega65" OR ${PLATFORM} STREQUAL "c64")
    target_compile_options(vim PRIVATE
        -fdiagnostics-show-option
        -fcolor-diagnostics
        -Wno-c23-extensions
        -fno-signed-zeros
        -fno-spell-checking
        -fno-fixed-point
    )
endif()
