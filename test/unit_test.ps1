# A Test Script on MS Windows
# Unit Test for All of Algorithms

# Test File
$t_file = "../huffman/zlibh.c"

# Compile the test program
emake .\bwt_stream.mak | Out-Null
emake .\mtf_stream.mak | Out-Null
emake .\rlc_stream.mak | Out-Null
emake .\huff_stream.mak | Out-Null

# Every Stream Test
./bwt_st.exe $t_file
./mtf_st.exe $t_file
./rlc_st.exe $t_file
./huff_st.exe $t_file

# Remove Binary files
Remove-Item *.exe | Out-Null
Remove-Item ./obj/*.* -Recurse | Out-Null

# EOF
