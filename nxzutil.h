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
			if(n == 0u) { return ; }
			ulength = n; uptr = new uint8_t[ulength]{0u}; posbuff = 0u;
		}

		VLBUFF(VLBUFF& x, uint32_t begin, uint32_t end)
		{
			ulength = begin - end;
			uptr = new uint8_t[ulength];

			std::copy(x.uptr+begin, x.uptr+end, uptr);
		}

		VLBUFF()
		{
			uptr = nullptr; ulength = 0u; posbuff = 0u;
		}
		
		~VLBUFF(void)
		{
			delete[] uptr; ulength = 0u; posbuff = 0u;
		}

		int8_t allocate(uint32_t n)
		{
			if(n == 0u) { return -1; }
			if(n == ulength) { posbuff = 0u; return 0u; }
			if(uptr != nullptr) { delete[] uptr; }

			ulength = n; uptr = new uint8_t[ulength]{0u}; posbuff = 0u;
			return 0u;
		}

		void allocate(void)
		{
			if(!is_null()) { return ; }

			uptr = new uint8_t[ulength]{0u};
		}
		
		void vlcopy(void* begin, uint32_t n)
		{
			std::memcpy((void*)(uptr+posbuff), begin, n);
			posbuff += n;
		}

		void flush(void)
		{
			if(is_null() || empty()) { return ; }

			for(uint32_t i = 0; i < ulength; i++) { uptr[i] = 0u; }
		}

		bool empty(void) { return ulength == 0u; }

		bool is_null(void) { return uptr == nullptr; }

		uint8_t& operator[](uint32_t n) { return uptr[n]; }

	private:
		uint32_t posbuff;
	};

	/* Command-line options Structure */
	struct CLIOPS
	{
		bool isLogging;
		bool isRemoveFile;
	};
}

#endif /*__NXZIP_UTILITY_H*/

/* End of File */
