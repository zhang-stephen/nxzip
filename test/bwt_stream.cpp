/**
 * A test for BWT Stream
 */

#include <iostream>

#include "../bwt/bwt.h"
#include "file.h"

using namespace std;
using namespace NXZIP;
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
	uint8_t *out_buff = new uint8_t[102400]; 

	/* BWT Stream Test */
	while(true)
	{
		/* read file */
		rxb = myfile.read(in_buff, 102400);
		tmpbsize = (rxb == 0 ? 102400 : rxb);

		/* allocate BWT class */
		BWT xbwt((uint32_t)tmpbsize);

		/* Burrows-Wheeler Transform */
		NXZ_BWTransform2(in_buff, xbwt.length, &xbwt);

		/* Inverse Burrows-Wheeler Transform */
		NXZ_BWTransform_Inverse2(xbwt.cstr, xbwt.length, xbwt.index, out_buff);

		/* Compare */
		if(memcmp((const void*)in_buff, (const void*)out_buff, tmpbsize) == 0) 
		{
			cout << "BWT is Correct" << endl;
		}
		else
		{
			cout << "BWT is ERROR" << endl;
		}

		if(rxb != 0) { break ; }
	}

	delete[] in_buff;
	delete[] out_buff;

	return 0u;
}

//EOF
