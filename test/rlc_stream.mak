# emake configuration
# for RLE Stream Test

# Create eXcutable File
mode: exe

# Flags of g++
flag: -g, -std=c++14

# Include
inc: ./, ../, ../rlc/

# Source
src: ./rlc_stream.cpp
src: ../rlc/rlc.cpp

# Temporary Folder
int: ./obj

# File name
out: rlc_st.exe

# Libraries
link: stdc++

# EOF
