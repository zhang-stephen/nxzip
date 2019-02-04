/**
 * BWT Algorithm (header file)
 * 2019-02-02
 */

#ifndef __BWT_H
#define __BWT_H

#include <cstdint>

namespace NXZIP
{
	class BWT
	{
	public:
		uint8_t* cstr;			/* !< The Result Array in C Style */
		uint32_t index;			/* !< The position of flag character */
		uint32_t length;		/* !< The Length of Result Array */

		BWT(uint32_t size);
		~BWT(void);
	};

	bool NXZ_BWTransform(uint8_t* srcArray, BWT* bwt);
	bool NXZ_BWTransform_Inverse(BWT* ibwt, uint8_t* dstArray);
}

#endif /*__BWT_H*/

//EOF
