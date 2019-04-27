/**
 * A Simple Utility Header-File Only Lib for NXZIP
 * 2019-04-07
 */

#ifndef __NXZIP_UTILITY_H
#define __NXZIP_UTILITY_H

#include <iostream>
#include <cstring>
#include <cstdio>

namespace NXZIP::utility
{
	/* variable length buffer class for RLE/Huffman */
	class VLBUFF
	{
	public:
		/* variable length buffer class for RLE/Huffman */
		uint8_t* uptr;
		uint32_t ulength;

		VLBUFF(uint32_t n)
		{
			ulength = n; uptr = new uint8_t[ulength]{0u}; posbuff = 0u;
		}

		VLBUFF()
		{
			uptr = nullptr; ulength = 0u; posbuff = 0u;
		}
		
		~VLBUFF(void)
		{
			delete[] uptr; ulength = 0u; posbuff = 0u;
		}

		void allocate(uint32_t n)
		{
			if(n == ulength) { return; }
			if(uptr != nullptr) { delete[] uptr; }

			ulength = n; uptr = new uint8_t[ulength]{0u}; posbuff = 0u;
		}
		void vlcopy(void* begin, uint32_t n)
		{
			std::memcpy((void*)(uptr+posbuff), begin, n);
			posbuff += n;
		}

	private:
		uint32_t posbuff;
	};

	/* Command-line options Structure */
	struct CLIOPS
	{
		bool isLogging;
		bool isRemoveFile;

		uint8_t encoding;
	};
}

#endif /*__NXZIP_UTILITY_H*/

/* End of File */
