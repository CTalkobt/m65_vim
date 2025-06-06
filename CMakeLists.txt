cmake_minimum_required(VERSION 3.18)

## My Environment settings.
set(CMAKE_C_COMPILER /home/duck/m65/c/llvm-mos/bin/mos-mega65-clang)
set(CMAKE_CXX_COMPILER /home/duck/m65/c/llvm-mos/bin/mos-mega65-clang++)
set(CMAKE_PREFIX_PATH /home/duck/m65/c/llvm-mos)
## End my environment settings.

find_package(llvm-mos-sdk REQUIRED)
set(LLVM_MOS_PLATFORM mega65)

project(vim)

include(CPM.cmake)
CPMAddPackage(NAME mega65libc GITHUB_REPOSITORY mega65/mega65-libc GIT_TAG development)

add_executable(vim
        src/vim.c
        src/cmd.c
        src/render.c
        src/lib/itoa.c
        src/lib/m65/txtio.c
)
target_link_libraries(vim mega65libc)
