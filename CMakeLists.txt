# SPDX-License-Identifier: MIT
# Copyright (c) 2020-2026 RVX Project Contributors

cmake_minimum_required(VERSION 3.28)

project(RVX LANGUAGES C ASM)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Define as INTERFACE library
add_library(rvx_sdk INTERFACE)

# Include directories
target_include_directories(rvx_sdk
  INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# Compiler options
target_compile_options(rvx_sdk
  INTERFACE
  -Wall
  -Wextra
  -Wpedantic
  -mstrict-align
  -ffreestanding
  -fno-builtin
  -ffunction-sections
  -fdata-sections
  -mabi=ilp32
  -mno-relax
)

target_link_options(rvx_sdk
  INTERFACE
  -Wl,--gc-sections
  -mstrict-align
  -mabi=ilp32
  -Wl,--no-warn-rwx-segments
  -nostartfiles
)

include(${CMAKE_CURRENT_LIST_DIR}/cmake/RVXBootImage.cmake)

configure_file(
  ${CMAKE_CURRENT_LIST_DIR}/cmake/RVXBootImage.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/RVXBootImage.cmake
  COPYONLY
)

configure_file(
  ${CMAKE_CURRENT_LIST_DIR}/cmake/templates/RVXUninstall.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/RVXUninstall.cmake
  @ONLY
)

configure_file(
  ${CMAKE_CURRENT_LIST_DIR}/cmake/templates/RVXWriteInstallManifest.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/RVXWriteInstallManifest.cmake
  @ONLY
)

add_custom_target(uninstall
  COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/RVXUninstall.cmake
)

install(TARGETS rvx_sdk EXPORT RVXTargets)

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(DIRECTORY startup/ DESTINATION ${CMAKE_INSTALL_DATADIR}/rvx/startup)
install(DIRECTORY ld/ DESTINATION ${CMAKE_INSTALL_DATADIR}/rvx/ld)

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/RVXBootImage.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/RVXUninstall.cmake
  ${CMAKE_CURRENT_LIST_DIR}/cmake/RVXToolchain.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/RVX
)

configure_package_config_file(
  ${CMAKE_CURRENT_LIST_DIR}/cmake/templates/RVXConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/RVXConfig.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/RVX
)

write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/RVXConfigVersion.cmake
  VERSION 0.1.0
  COMPATIBILITY SameMajorVersion
)

install(EXPORT RVXTargets
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/RVX
)

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/RVXConfig.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/RVXConfigVersion.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/RVX
)

# Persist the install manifest in the install tree so uninstall can work
# even after the build directory is deleted.
install(SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/RVXWriteInstallManifest.cmake)

export(EXPORT RVXTargets
  FILE ${CMAKE_CURRENT_BINARY_DIR}/RVXTargets.cmake
)