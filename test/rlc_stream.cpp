/**
 * A test for BWT Stream
 */

#include <iostream>

#include "../rlc/rlc.h"
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

	uint8_t *in_buff = new uint8_t[102400];
	VLBUFF rlc_buff, out_buff;

	/* Run-Length Encodig Stream Test */
	while(true)
	{
		/* read file */
		rxb = myfile.read(in_buff, 102400);
		tmpbsize = (rxb == 0 ? 102400 : rxb);

		/* Run-Length Encoding */
		NXZ_mRunLength_Encoding(in_buff, tmpbsize, &rlc_buff);

		/* Run-Length Decoding */
		NXZ_mRunLength_Decoding(rlc_buff.uptr, rlc_buff.ulength, &out_buff);

		/* Compare */
		if(memcmp((const void*)in_buff, (const void*)out_buff.uptr, tmpbsize) == 0) 
		{
			cout << "RLE is Okay" << endl;
		}
		else
		{
			cout << "RLE is ERROR" << endl;
		}

		if(rxb != 0) { break ; }
	}

	delete[] in_buff;

	return 0u;
}

//EOF
