# make the asm file to static library

cmake_minimum_required(VERSION 3.10)

project(yasmCMAKE)

find_program(YASM_EXECUTABLE NAMES yasm)

aux_source_directory(./ SRC_LIST)

if(UNIX)
	add_custom_command(
		OUTPUT crc32asm.o
		COMMAND ${YASM_EXECUTABLE}
		ARGS -f x64 -f elf64 -X gnu -g dwarf2 -D LINUX -o crc_iscsi_v_pcl.o ${SRC_LIST}/crc_iscsi_v_pcl.asm
	)
elseif(WIN32)
	add_custom_command(
		OUTPUT crc32asm.o 
		COMMAND ${YASM_EXECUTABLE}
		ARGS -Xvc -f x64 -rnasm -pnasm -o crc32asm.o -g cv8 ${SRC_LIST}/crc_iscsi_v_pcl.asm
	)
else()
	message(
		FATAL_ERROR
		"Your platform is not supported!"
	)
endif()

add_library(crc32 ${SRC_LIST})
set_target_properties(crc32 PROPERTIES LINKER_LANGUAGE CXX)

