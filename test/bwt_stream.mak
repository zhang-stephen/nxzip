# emake configuration
# for BWT Stream Test

# Create eXcutable File
mode: exe

# Flags of g++
flag: -g, -std=c++14

# Include
inc: ./, ../bwt/

# Source
src: ./bwt_stream.cpp
src: ../bwt/bwt2.cpp

# Temporary Folder
int: ./obj

# File name
out: bwt_st.exe

# Libraries
link: stdc++

# EOF
