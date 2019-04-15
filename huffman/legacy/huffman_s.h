/**
 * Static Huffman Coding Algorithm(header file)
 * 2019-03-28
 */

#ifndef __HUFFMAN_S_H
#define __HUFFMAN_S_H

#include <cstdint>
#include "../../nxzutil.h"

namespace NXZIP
{
	class sHuffman
	{
	public:
		uint32_t elemFreq[256u];	/* !< probability model */

		uint32_t huffCodeLen;		/* !< bit length of huffman code */
		uint8_t* huffCode;			/* !< huffman code bit(need to reduce to byte) */

		void allocateCodePtr(void);
		~sHuffman(void);
	};

	bool NXZ_sHuffmanEncode(uint8_t* srcArr, uint32_t srcLength, sHuffman* xhuff);
	bool NXZ_sHuffmanDecode(uint8_t* dstArr, uint32_t dstLength, uint32_t* elemFreq, uint8_t* code, uint32_t cLength);

	/* BitStream Operations, without RLE */
	bool NXZ_Huffman_RuduceByte2Bit(uint8_t* xCode, uint32_t codeLength, utility::VLBUFF* nBuff);
	bool NXZ_Huffman_ExpandBit2Byte(uint8_t* xBytes, uint32_t byteSize, utility::VLBUFF* cBuff);
}

#endif /*__HUFFMAN_S_H*/

/* End of File */
