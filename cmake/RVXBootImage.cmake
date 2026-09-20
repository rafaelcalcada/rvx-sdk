# SPDX-License-Identifier: MIT
# Copyright (c) 2020-2026 RVX Project Contributors

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