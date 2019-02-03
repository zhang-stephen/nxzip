# nxzip compiling configuration via cmake 3.10
# this project used the c++17(gnu++17), compiled via GNU C/C++ Compiler v8.2.0
# 2019-02-03

# minimal cmake version 
cmake_minimum_required(VERSION 3.10)

# Project NXZIP
project(nxzip)

# ISO C++17
set(CMAKE_CXX_STANDARD 17)

# Include Path
include_directories(
	./3rdlib			# the Third-Party Libraries
	./bwt				# the BWT header and source file
	./movetofront		# the MTF header and source file
	./					# main directory
)

# Source Path
aux_source_directory(./bwt SRC_LIST)
aux_source_directory(./movetofront SRC_LIST)
aux_source_directory(./ SRC_LIST)

# Target Building
add_executable(nxzip-debug ${SRC_LIST})

# Link Libs

# EOF
