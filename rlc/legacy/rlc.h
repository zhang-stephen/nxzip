/**
 * Run-Length Coding Algorithms(header file)
 * 2019-02-27
 */

#ifndef __RLC_H
#define __RLC_H

#include <cstdint>

#include "../../nxzutil.h"

namespace NXZIP
{
	bool NXZ_RunLengthEncode(uint8_t* srcBuff, uint32_t srcLength, utility::VLBUFF* codeBuff);
	bool NXZ_RunLengthDecode(uint8_t* rlcBuff, uint32_t rlcLength, utility::VLBUFF* yBuff);
}

#endif /*__RLC_H*/

/* End of File */
