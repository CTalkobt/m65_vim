cmake_minimum_required(VERSION 3.18)

## My Environment settings.
set(CMAKE_C_COMPILER /home/duck/m65/c/llvm-mos/bin/mos-mega65-clang)
#set(CMAKE_CXX_COMPILER /home/duck/m65/c/llvm-mos/bin/mos-mega65-clang++)
set(CMAKE_PREFIX_PATH /home/duck/m65/c/llvm-mos)


## set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --extra-warnings -fno-signed-char -fdiagnostics-show-option -fcolor-diagnostics")
## End my environment settings.

find_package(llvm-mos-sdk REQUIRED)
set(LLVM_MOS_PLATFORM mega65)

project(vim)

option(ENABLE_DEBUG "Enable debug messages for the main vim executable" ON)

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
        src/editor.c
        src/render.c
        src/line.c
        src/buffer.c
        src/lib/itostr.c
        src/lib/itostr.h
        src/lib/m65/debug.h
        src/lib/m65/kbd.h
        src/lib/m65/kbd.c
        src/lib/m65/kernal.c
        src/lib/m65/kernal.h
        src/lib/m65/screen.c
        src/lib/m65/screen.h
)
target_link_libraries(vim mega65libc)
target_compile_definitions(vim PRIVATE DEBUG_ON)                                                                                                         


add_executable(vim_test
        test/test.c
        test/suites/test_itostr.c
        test/suites/test_cmd.c
        test/suites/test_vim.c
        test/suites/test_render.c
        test/suites/test_insert_mode.c
        src/cmd.c
        src/editor.c
        src/line.c
        src/buffer.c
        src/lib/itostr.c
        src/lib/m65/kernal.c
        src/lib/m65/screen.c
        src/lib/m65/kbd.c
)
target_link_libraries(vim_test mega65libc)
target_include_directories(vim_test PRIVATE src)
target_compile_definitions(vim_test PRIVATE DEBUG_ON)

add_executable(vim_test_cmd_read
        test/suites/test_cmd_read.c
        src/cmd.c
        src/line.c
        src/buffer.c
        src/lib/itostr.c
        # Note: We do NOT include kernal.c here, so the mocks are used.
)
target_link_libraries(vim_test_cmd_read mega65libc)
target_include_directories(vim_test_cmd_read PRIVATE src)
target_compile_definitions(vim_test_cmd_read PRIVATE DEBUG_ON)

add_executable(vim_test_render
        test/suites/test_render_mocked.c
        src/render.c
        # Note: We do NOT include screen.c or itostr.c here, so the mocks are used.
)
target_link_libraries(vim_test_render mega65libc)
target_include_directories(vim_test_render PRIVATE src)
target_compile_definitions(vim_test_render PRIVATE DEBUG_ON)

add_executable(vim_test_cmd_write
        test/suites/test_cmd_write.c
        src/cmd.c
        src/buffer.c
        src/line.c
        # Mocks are in the test file
)
target_link_libraries(vim_test_cmd_write mega65libc)
target_include_directories(vim_test_cmd_write PRIVATE src)
target_compile_definitions(vim_test_cmd_write PRIVATE DEBUG_ON)

add_executable(vim_test_commands
        test/suites/test_commands.c
        src/editor.c
        src/cmd.c
        src/line.c
        src/buffer.c
)
target_link_libraries(vim_test_commands mega65libc)
target_include_directories(vim_test_commands PRIVATE src)

add_executable(vim_test_directory_listing
        test/suites/test_directory_listing.c
        src/cmd.c
)
target_link_libraries(vim_test_directory_listing mega65libc)
target_include_directories(vim_test_directory_listing PRIVATE src)

add_executable(vim_test_scrolling
        test/suites/test_scrolling.c
        src/cmd.c
        src/buffer.c
        src/line.c
)
target_link_libraries(vim_test_scrolling mega65libc)
target_include_directories(vim_test_scrolling PRIVATE src)


target_compile_definitions(vim_test_commands PRIVATE DEBUG_ON)





# Let's set a rather loud warning level
target_compile_options(vim PRIVATE
        -Wno-c23-extensions
        -Wshadow # warn the user if a variable declaration shadows one from a parent context
        -Wcast-align # warn for potential performance problem casts
        -Wunused # warn on anything being unused
        -Wnull-dereference # warn if a null dereference is detected
        -Wformat=2 # warn on security issues around functions that format output (ie printf)
        -Wimplicit-fallthrough # warn on statements that fallthrough without an explicit annotation
        -Wno-incompatible-pointer-types-discards-qualifiers

        -Wno-sign-conversion
        -Wno-int-conversion
        -Wno-pointer-sign

        -fno-signed-char
        -fno-signed-zeros
        -fno-spell-checking
        -fno-fixed-point
)
