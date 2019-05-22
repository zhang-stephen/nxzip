# emake configuration
# for MTF Stream Test

# Create eXcutable File
mode: exe

# Flags of g++
flag: -g, -std=c++14

# Include
inc: ./, ../movetofront/

# Source
src: ./mtf_stream.cpp
src: ../movetofront/mtf.cpp

# Temporary Folder
int: ./obj

# File name
out: mtf_st.exe

# Libraries
link: stdc++

# EOF
