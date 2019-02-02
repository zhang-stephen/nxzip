# The Nxzip Compiling Configurations
# make by emake(https://github.com/skywind3000/emake), and will use cmake or makefile
# compiled for DEBUG

# Output file Type
mode: exe

# Output file Name
out: ../build/nxzip-debug.exe

# Temporary Path
int: ../build/objs/debug/

# Source File
src: ../main.cpp

# Include Path
inc: ../

# Extra Libraries Path
lib: 

# Linked Libraries
link: stdc++, 

# the Compiling Flags to GCC
flag: --std=gnu++17, -g, -O0, -Wall

# EOF
