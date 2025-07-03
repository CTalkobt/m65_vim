cmake_minimum_required(VERSION 3.18)

## My Environment settings.
set(CMAKE_C_COMPILER /home/duck/m65/c/llvm-mos/bin/mos-mega65-clang)
set(CMAKE_CXX_COMPILER /home/duck/m65/c/llvm-mos/bin/mos-mega65-clang++)
set(CMAKE_PREFIX_PATH /home/duck/m65/c/llvm-mos)


## set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --extra-warnings -fno-signed-char -fdiagnostics-show-option -fcolor-diagnostics")
## End my environment settings.

find_package(llvm-mos-sdk REQUIRED)
set(LLVM_MOS_PLATFORM mega65)

project(vim)

#include(CPM.cmake)
include(FetchContent)
FetchContent_Declare(
        CPM
        GIT_REPOSITORY https://github.com/cpm-cmake/CPM.cmake
        GIT_TAG        v0.42.0 # Replace with the desired version
)
FetchContent_MakeAvailable(CPM)
#
# Use my mega65-libc fork for now.
# - PRs may still be pending. At some future point, we'll use:
#CPMAddPackage(NAME mega65libc GITHUB_REPOSITORY mega65/mega65-libc GIT_TAG development)
CPMAddPackage(NAME mega65libc GITHUB_REPOSITORY CTalkobt/mega65-libc GIT_TAG development)

set(CMAKE_C_FLAGS "\
    ${CMAKE_C_FLAGS}\
    -fno-signed-char\
    -fdiagnostics-show-option\
    -fcolor-diagnostics\
    -fno-signed-zeros\
    -fapprox-func\
    -ffast-math\
    -finline-functions\
    -fjump-tables\
    -fverbose-asm\
")
#add_compile_options(
#        -S    ## Generate intermediate language. source.
#        --verbose-asm
#)



#-Wall
#-Wno-language-extension-token
#-Wshadow # warn the user if a variable declaration shadows one from a parent context
#-Wcast-align # warn for potential performance problem casts
#-Wunused # warn on anything being unused
#-Wnull-dereference # warn if a null dereference is detected
#-Wformat=2 # warn on security issues around functions that format output (ie printf)
#-Wimplicit-fallthrough # warn on statements that fallthrough without an explicit annotation


add_executable(vim
        src/vim.c
        src/cmd.c
        src/render.c
        src/lib/itoa.c
        src/lib/m65/debug.h
        src/lib/m65/kbd.h
        src/lib/m65/kbd.c
        src/lib/m65/kernal.c
        src/lib/m65/kernal.h
        src/lib/m65/screen.c
        src/lib/m65/screen.h
)
target_link_libraries(vim mega65libc)


add_executable(test
        src/test.c
        src/lib/m65/kernal.c
        src/lib/m65/kernal.h
        src/lib/m65/screen.h
        src/lib/m65/screen.c
        src/lib/m65/kbd.c
        src/lib/m65/kbd.h
)
target_link_libraries(test mega65libc)

# Let's set a rather loud warning level
target_compile_options(vim PRIVATE
        -Wno-c23-extensions
        -Wshadow # warn the user if a variable declaration shadows one from a parent context
        -Wcast-align # warn for potential performance problem casts
        -Wunused # warn on anything being unused
        -Wnull-dereference # warn if a null dereference is detected
        -Wformat=2 # warn on security issues around functions that format output (ie printf)
        -Wimplicit-fallthrough # warn on statements that fallthrough without an explicit annotation

        -Wno-sign-conversion
        -Wno-int-conversion
        -Wno-pointer-sign
)

