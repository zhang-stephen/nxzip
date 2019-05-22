/**
 * A test for BWT Stream
 */

#include <iostream>

#include "../huffman/huffman.h"
#include "../nxzutil.h"
#include "file.h"

using namespace std;
using namespace NXZIP;
using namespace NXZIP::utility;
using namespace NXZIP::Stream;

int main(int argc, char** argv)
{
	if(argv[1] == nullptr) { return -1; }

	if(!is_file_exists(argv[1])) { return -2; }
	if(!is_regular_file(argv[1])) { return -3; }

	/* Open file to read */
	_io_file myfile(argv[1], "r");
	if(!myfile.is_open()) { return -4; }

	uint32_t tmpfsize = file_size(argv[1]), tmpbsize = 0u;
	int32_t rxb = 0;

	VLBUFF in_buff(102400), out_buff(102400), huff_buff(102400);

	/* BWT Stream Test */
	while(true)
	{
		/* read file */
		rxb = myfile.read(in_buff.uptr, 102400);
		tmpbsize = (rxb == 0 ? 102400 : rxb);
		huff_buff.ulength = tmpbsize;

		/* Huffman Encoding */
		NXZ_Huffman_Encoding(&in_buff, &huff_buff);

		/* Huffman Decoding */
		NXZ_Huffman_Decoding(&huff_buff, &out_buff);

		/* Compare */
		if(memcmp((const void*)in_buff.uptr, (const void*)out_buff.uptr, tmpbsize) == 0) 
		{
			cout << "Huffman is Okay" << endl;
		}
		else
		{
			cout << "Huffman is ERROR" << endl;
		}

		if(rxb != 0) { break ; }
	}

	return 0u;
}
