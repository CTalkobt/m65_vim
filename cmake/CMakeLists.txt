# cmake/distclean.cmake
cmake_minimum_required(VERSION 3.18)
project(vim-cleanup)

# This target performs a "deep clean", removing build directories and binaries.
add_custom_target(distclean
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_SOURCE_DIR}/bin/vim_ubuntu
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_SOURCE_DIR}/bin/vim_c64
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_SOURCE_DIR}/bin/vim_mega65
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_SOURCE_DIR}/cmake-build-ubuntu
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_SOURCE_DIR}/cmake-build-c64
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_SOURCE_DIR}/cmake-build-m65
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_SOURCE_DIR}/cmake-build-ubuntu-memcheck
    COMMENT "Cleaning all build directories and binaries..."
)
