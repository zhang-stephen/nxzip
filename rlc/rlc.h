/**
 * New Run-Length Encoding for output sequence of MTF(header file)
 * 2019-04-12
 */

/**
 * @fomula	cc = count + 1 - 2 ^ floor(log2(count + 1))
 * @param	cc: the number to store
 * @param	count: the count of consequent zeros
 * @param	floor(log2(count + 1)): bits count of cc
 */

/**
 * Escape Character 1(for zeros subsequence)
 * +-------------------------------------------------------+
 * | bit7 | bit6 | bit5 | bit4 | bit3 | bit2 | bit1 | bit0 |
 * +------|------|------|------|------|------|------|------+
 * |  F=1 |  count of effective bits  | higher 3 bits of cc|
 * +-------------------------------------------------------+
 * 
 * Escape Character 2(for zeros subsequence)
 * +-------------------------------------------------------+
 * | bit7 | bit6 | bit5 | bit4 | bit3 | bit2 | bit1 | bit0 |
 * +------|------|------|------|------|------|------|------+
 * |               lower 8 bits of cc                      |
 * +-------------------------------------------------------+
 * 
 * Escape Character 1(for non-zeros subsequence)
 * +-------------------------------------------------------+
 * | bit7 | bit6 | bit5 | bit4 | bit3 | bit2 | bit1 | bit0 |
 * +------|------|------|------|------|------|------|------+
 * |  F=0 |  count of next non-zeros subsequence(max: 127) |
 * +-------------------------------------------------------+
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
