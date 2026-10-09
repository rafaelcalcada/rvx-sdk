# SPDX-License-Identifier: MIT
# Copyright (c) 2020-2026 RVX Project Contributors

set(RVX_INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/include")
set(RVX_STARTUP_DIR "${CMAKE_CURRENT_LIST_DIR}/startup")
set(RVX_LINKER_DIR "${CMAKE_CURRENT_LIST_DIR}/ld")
set(RVX_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/source")

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  message(FATAL_ERROR "RVX requires a CMake toolchain file.\n"
    "The toolchain file configures the RISC-V toolchain for cross-compilation for RVX.\n"
    "Please clean the build directory and call CMake again with "
    "-DCMAKE_TOOLCHAIN_FILE=<sdk-path>/RVXToolchain.cmake")
endif()

if(NOT DEFINED RVX_ARCH)
  if(DEFINED RVX_ENABLE_ZMMUL AND RVX_ENABLE_ZMMUL)
    set(RVX_ARCH "rv32i_zicsr_zmmul")
    message(STATUS "Selected RISC-V architecture for RVX: ${RVX_ARCH}")
    message(STATUS "Zmmul extension enabled.")
  else()
    set(RVX_ARCH "rv32i_zicsr")
    message(STATUS "Selected RISC-V architecture for RVX: ${RVX_ARCH} (default)")
    message(STATUS "Zmmul extension is disabled (default).")
  endif()
endif()

if(NOT DEFINED RVX_RAM_SIZE_IN_BYTES)
  set(RVX_RAM_SIZE_IN_BYTES 8192)
  message(STATUS "Using RVX_RAM_SIZE_IN_BYTES: ${RVX_RAM_SIZE_IN_BYTES} bytes (default)")
else()
  message(STATUS "Using RVX_RAM_SIZE_IN_BYTES: ${RVX_RAM_SIZE_IN_BYTES} bytes")
endif()

if(NOT DEFINED RVX_MIN_FREE_SPACE_IN_BYTES)
  set(RVX_MIN_FREE_SPACE_IN_BYTES 2048)
  message(STATUS "Using RVX_MIN_FREE_SPACE_IN_BYTES: ${RVX_MIN_FREE_SPACE_IN_BYTES} bytes (default)")
else()
  message(STATUS "Using RVX_MIN_FREE_SPACE_IN_BYTES: ${RVX_MIN_FREE_SPACE_IN_BYTES} bytes")
endif()

# Keep repeated package discovery from redefining the target or duplicating settings.
if(TARGET rvx_sdk)
  return()
endif()

add_library(rvx_sdk INTERFACE IMPORTED GLOBAL)

target_include_directories(rvx_sdk INTERFACE "${RVX_INCLUDE_DIR}")

target_compile_options(rvx_sdk INTERFACE
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
  "-march=${RVX_ARCH}"
)

target_link_options(rvx_sdk INTERFACE
  -Wl,--gc-sections
  -mstrict-align
  -mabi=ilp32
  -Wl,--no-warn-rwx-segments
  -nostartfiles
  "-march=${RVX_ARCH}"
  "-Wl,--defsym=__rvx_ram_size=${RVX_RAM_SIZE_IN_BYTES}"
  "-Wl,--defsym=__rvx_min_free_space=${RVX_MIN_FREE_SPACE_IN_BYTES}"
  "-T${RVX_LINKER_DIR}/rvx.ld"
)

target_sources(rvx_sdk INTERFACE
  "${RVX_STARTUP_DIR}/rvx_startup.S"
  "${RVX_SOURCE_DIR}/rvx_i2c.c"
)

function(rvx_generate_boot_image TARGET_NAME)
  file(RELATIVE_PATH rel_path ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

  add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O verilog --verilog-data-width=4 $<TARGET_FILE:${TARGET_NAME}> $<TARGET_FILE_DIR:${TARGET_NAME}>/${TARGET_NAME}.mem
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${TARGET_NAME}> $<TARGET_FILE_DIR:${TARGET_NAME}>/${TARGET_NAME}.bin
    COMMAND ${CMAKE_OBJDUMP} -D $<TARGET_FILE:${TARGET_NAME}> > $<TARGET_FILE_DIR:${TARGET_NAME}>/${TARGET_NAME}.disasm
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${TARGET_NAME}> $<TARGET_FILE_DIR:${TARGET_NAME}>/${TARGET_NAME}.elf
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMAND ${CMAKE_COMMAND} -E echo "Generated files:"
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMAND ${CMAKE_COMMAND} -E echo "Boot image \\(SPI flash\\): ${rel_path}/${TARGET_NAME}.bin"
    COMMAND ${CMAKE_COMMAND} -E echo "Boot image \\(FPGA/TCM\\):  ${rel_path}/${TARGET_NAME}.mem"
    COMMAND ${CMAKE_COMMAND} -E echo "ELF binary:             ${rel_path}/${TARGET_NAME}.elf"
    COMMAND ${CMAKE_COMMAND} -E echo "Disassembly:            ${rel_path}/${TARGET_NAME}.disasm"
    COMMAND ${CMAKE_COMMAND} -E echo ""
  )

  set_property(TARGET ${TARGET_NAME} APPEND PROPERTY ADDITIONAL_CLEAN_FILES
    $<TARGET_FILE_DIR:${TARGET_NAME}>/${TARGET_NAME}.bin
    $<TARGET_FILE_DIR:${TARGET_NAME}>/${TARGET_NAME}.elf
    $<TARGET_FILE_DIR:${TARGET_NAME}>/${TARGET_NAME}.mem
    $<TARGET_FILE_DIR:${TARGET_NAME}>/${TARGET_NAME}.disasm
  )
endfunction()