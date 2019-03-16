/**
 * Move to Front Algorithm(header file)
 * 2019-02-02
 */

#ifndef __MOVETOFRONT_H
#define __MOVETOFRONT_H

#include <cstdint>

namespace NXZIP
{
	bool NXZ_MoveToFront(const uint8_t* nString, uint32_t nSize, uint32_t* rArray);
	bool NXZ_MoveToFront_Inverse(const uint32_t* nArray, uint32_t nSize, uint8_t* rString);
}

#endif /*__MOVETOFRONT_H*/

//EOF
