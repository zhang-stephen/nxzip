/**
 * A New Implementation of Huffman Algorithm 
 * Reference: 
 * 2019-05-15
 */

#ifndef __z_HUFFMAN_H
#define __z_HUFFMAN_H

#include <cstdint>
#include "../nxzutil.h"

namespace NXZIP
{
	typedef utility::VLBUFF uvbuff;

	bool NXZ_Huffman_Encoding(uvbuff* inbuff, uvbuff* outbuff);
	bool NXZ_Huffman_Decoding(uvbuff* inbuff, uvbuff* outbuff);
}

#endif /*__z_HUFFMAN_H*/

/* End of File */
