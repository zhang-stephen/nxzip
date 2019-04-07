/**
 * A Simple Utility Header-File Only Lib for NXZIP
 * 2019-04-07
 */

#ifndef __NXZIP_UTILITY_H
#define __NXZIP_UTILITY_H

#include <iostream>

namespace NXZIP::utility
{
	/* variable length buffer class for RLE/Huffman */
	class vlbuff
	{
	public:
		uint8_t* uptr;
		uint32_t ulength;

		vlbuff(uint32_t n)
		{
			ulength = n; uptr = new uint8_t[ulength]{0u};
		}
		
		~vlbuff(void)
		{
			delete[] uptr; ulength = 0u;
		}

		void allocate(uint32_t n)
		{
			if(n == ulength) { return; }
			if(uptr != nullptr) { delete[] uptr; }
			ulength = n; uptr = new uint8_t[ulength]{0u};
		}

		void flush(void)
		{
			delete[] uptr;
			uptr = new uint8_t[ulength]{0u};
		}
	};
}

#endif /*__NXZIP_UTILITY_H*/

/* End of File */
