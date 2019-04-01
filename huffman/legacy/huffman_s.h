/**
 * Static Huffman Coding Algorithm(header file)
 * 2019-03-28
 */

#ifndef __HUFFMAN_S_H
#define __HUFFMAN_S_H

#include <cstdint>

namespace NXZIP
{
	class sHuffman
	{
	public:
		uint32_t elemFreq[256u];

		uint32_t huffCodeLen;
		uint8_t* huffCode;

		void allocateCodePtr(void);
		~sHuffman(void);
	};

	bool NXZ_sHuffmanEncode(uint8_t* srcArr, uint32_t srcLength, sHuffman* xhuff);
	bool NXZ_sHuffmanDecode(uint8_t* dstArr, uint32_t dstLength, sHuffman* zhuff);

	bool NXZ_Huffman_RuduceByte2Bit(uint8_t* xCode, uint32_t codeLength, uint8_t*& nBuff, uint32_t& buffSize);
	bool NXZ_Huffman_ExpandBit2Byte(uint8_t* xBytes, uint32_t byteSize, uint8_t*& codeBuff, uint32_t codeLength);
}

#endif /*__HUFFMAN_S_H*/

/* End of File */
