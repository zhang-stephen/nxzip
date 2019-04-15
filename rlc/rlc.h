/**
 * New Run-Length Encoding for output sequence of MTF(header file)
 * 2019-04-12
 */

#ifndef __RLE_N_H
#define __RLE_N_H

#include <cstdint>
#include "../nxzutil.h"

namespace NXZIP
{
	bool NXZ_mRunLength_Encoding(uint8_t* src, uint32_t srcLength, utility::VLBUFF* rlcbuff);
	bool NXZ_mRunLength_Decoding(uint8_t* rlc, uint32_t rlcLength, utility::VLBUFF* oribuff);
}

#endif /*__RLE_N_H*/

/* End of File */
