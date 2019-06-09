# nxzip compiling configuration via cmake 3.10
# this project used the c++17(gnu++17), compiled via GNU C/C++ Compiler v8.2.0
# 2019-02-03

# minimal cmake version 
cmake_minimum_required(VERSION 3.10)

# Project NXZIP
project(nxzip)

# ISO C++17
set(CMAKE_CXX_STANDARD 11)					# C++ Standard
set(CMAKE_CXX_STANDARD_REQUIRED 11)			# Require C++17 to Compile(use the filesystem in c++17, not boost::filesystem)

option(NXZ_DEBUG "" ON)

# configuration files
configure_file(
	"${PROJECT_SOURCE_DIR}/config.h.in"
	"${PROJECT_SOURCE_DIR}/config.h"
)

add_subdirectory(./crc32)

# Include Path
include_directories(
	./3rdlib			# the Third-Party Libraries
	./bwt				# the BWT header and source file
	./CRC32c			# the crc-32c header and source file
	./file				# the file IO header and source file
	# ./huffman/legacy	# the Huffman-Coding header and source file
	./huffman 			# the new huffman encoding header and source file
	./logger			# the logger module
	./movetofront		# the MTF header and source file
	./rlc 				# the RLE header and source file
	./test 				# the test function
	./					# main directory
)

# Add Compiler Options
set(CMAKE_CXX_FLAGS "-msse4.2 -g -O3 ${CMAKE_CXX_FLAGS}")

# Source Path
aux_source_directory(./bwt SRC_LIST)			# The BWT Algorithm
aux_source_directory(./file SRC_LIST)			# The File IO Algorithm
aux_source_directory(./movetofront SRC_LIST)	# The MTF Algorithm
aux_source_directory(./rlc SRC_LIST)			# The RLEC Algorithm
# aux_source_directory(./huffman/legacy SRC_LIST) # The Huffman Algorithm
aux_source_directory(./huffman SRC_LIST)		# The New Huffman Algorithm
aux_source_directory(./test SRC_LIST)			# for Test
aux_source_directory(./ SRC_LIST)				# The MainCpp Path

# Target Building
add_executable(nxzip-debug ${SRC_LIST})			# Output Type: Executable Binary

# Link Libs
target_link_libraries(nxzip-debug crc32)		# crc32c lib with intel asm

# EOF
