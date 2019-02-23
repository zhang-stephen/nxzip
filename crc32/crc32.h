/**
 * the CRC-32C Checksum Algorithm(source file)
 * poly: 0x1EDC6F41/0x82F63B78
 * 2019-02-22
 */

#ifndef __CRC32_H
#define __CRC32_H

#include <cstdint>

namespace NXZIP
{
	uint32_t NXZ_CRC32_Calculate(uint32_t crc32c, uint8_t* src, uint32_t length);	
}


#endif /*__CRC32_H*/

/* End of File */
