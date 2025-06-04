cmake_minimum_required(VERSION 3.18)
set(LLVM_MOS_PLATFORM mega65)
find_package(llvm-mos-sdk REQUIRED)

include(CPM.cmake)
CPMAddPackage(NAME mega65libc GITHUB_REPOSITORY mega65/mega65-libc GIT_TAG development)

project(llvm-mos-sdk-foo)
add_executable(vim
        src/vim.c
        src/cmd.c
        src/render.c
        src/lib/itoa.c
        src/lib/m65/txtio.c
)
target_link_libraries(vim mega65libc)
