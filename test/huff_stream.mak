# emake configuration
# for Huffman Encoding Stream Test

# Create eXcutable File
mode: exe

# Flags of g++
flag: -g, -std=c++14

# Include
inc: ./, ../, ../huffman/

# Source
src: ./huff_stream.cpp
src: ../huffman/huffman.cpp
src: ../huffman/zlibh.c

# Temporary Folder
int: ./obj

# File name
out: huff_st.exe

# Libraries
link: stdc++

# EOF
