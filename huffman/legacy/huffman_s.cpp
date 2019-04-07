/**
 * Static Huffman Coding Algorithm(source file)
 * 2019-03-28
 */

#include <iostream>
#include <algorithm>
#include <vector>

#include "huffman_s.h"

namespace
{
	struct huffNode
	{
		uint32_t _hWeight;
		int32_t _hParent;
		uint16_t _hLeftChild, _hRightChild;
	};

	struct huffCode
	{
		uint16_t _huffNum;
		uint8_t* _huffKey;

		uint8_t* _huffBitsLen;
		uint8_t** _huffBits;

		huffCode(uint16_t n)
		{
			_huffNum = n;
			_huffKey = new uint8_t[n];
			_huffBitsLen = new uint8_t[n];
			_huffBits = new uint8_t*[n];
		}

		void assginMem(uint16_t num, uint16_t len)
		{
			_huffBits[num] = new uint8_t[len]{0u};
		}

		void freeBitsMem(void)
		{
			for(uint8_t i = 0u; i < _huffNum; i++)
			{
				delete[] _huffBits[i];
			}
		}

		~huffCode(void)
		{
			freeBitsMem();

			delete _huffKey;
			delete[] _huffBits;
			delete _huffBitsLen;
		}
	};
	
	void scanOriginSequence(uint8_t* originSeq, uint32_t seqLen, uint16_t& numLeafNode, uint32_t* huffTable);
	void selectMinNode(huffNode* hTree, uint16_t nRange, uint16_t& min1, uint16_t& min2);
	void createHuffmanTree(huffNode* hTree, uint16_t numLeafNode, uint32_t* huffTable);
	void getHuffmanCode(huffNode* hTree, uint16_t numLeafNode, huffCode& cTable);
	void toEncoding(uint8_t* srcArr, uint32_t srcLength, uint8_t* dstCode, uint32_t codeLength, huffCode* hTable);
	void toDecoding(huffNode* hTree, uint8_t* hCode, uint32_t codeLength, uint8_t* huffKeys, uint16_t numLeafNode, uint8_t *dstArr);
}

void NXZIP::sHuffman::allocateCodePtr(void) { huffCode = new uint8_t[huffCodeLen]; }

NXZIP::sHuffman::~sHuffman(void) { delete huffCode; }

/**
 * @brief	Scan the origin Sequence to create origin code table
 */
void scanOriginSequence(uint8_t* originSeq, uint32_t seqLen, uint16_t& numLeafNode, uint32_t* huffTable)
{
	/* Check */
	if(originSeq == nullptr || seqLen == 0u || huffTable == nullptr)
	{
		return ;
	}

	uint16_t tmp = 0u;

	while(tmp <= 0xFFu)
	{
		for(uint32_t i = 0u; i < seqLen; i++)
		{
			if(tmp == originSeq[i])
			{
				huffTable[tmp]++;
			}
		}

		if(huffTable[tmp] != 0u)
		{
			numLeafNode++;
		}

		tmp++;
	}
}

/**
 * @brief	find out two minimum node when creates the tree
 */
void selectMinNode(huffNode* hTree, uint16_t nRange, uint16_t& min1, uint16_t& min2)
{
	/* check */
	if(hTree == nullptr || nRange == 0u)
	{
		return ;
	}

	huffNode *x1 = nullptr, *x2 = nullptr, *t = nullptr;

	for(uint16_t i = 1u; i <= nRange; i++)
	{
		/* Parent Node is NULL */
		if(!hTree[i]._hParent)
		{
			if(x1 == nullptr)
			{
				x1 = hTree + i;
				continue;
			}

			if(x2 == nullptr)
			{
				x2 = hTree + i;

				/* smaller weight moves to left */
				if(x1->_hWeight > x2->_hWeight)
				{
					t = x1;
					x1 = x2;
					x2 = t;
				}

				continue;
			}

			if(x1 != nullptr && x2 != nullptr)
			{
				if(hTree[i]._hWeight <= x1->_hWeight)
				{
					x2 = x1; x1 = hTree + i;
				}
				else if(hTree[i]._hWeight < x2->_hWeight)
				{
					x2 = hTree + i;
				}
			}
		}
	}

	/* make sure that left node is leaf */
	if(x1 > x2)
	{
		min1 = x2 - hTree;
		min2 = x1 - hTree;
	}
	else
	{
		min1 = x1 - hTree;
		min2 = x2 - hTree;
	}
	
}

/**
 * @brief	create huffman tree(binary tree)
 */
void createHuffmanTree(huffNode* hTree, uint16_t numLeafNode, uint32_t* huffTable)
{
	/* check */
	if(hTree == nullptr || numLeafNode <= 1u || huffTable == nullptr)
	{
		return ;
	}

	uint16_t tmpIndex = 0u, numAllNode = numLeafNode * 2u - 1u;
	uint16_t minNode1 = 0u, minNode2 = 0u;

	/* Init Leaf Node */
	for(uint16_t i = 1u; i <= numLeafNode; ++i)
	{
		while(huffTable[tmpIndex++] == 0u);

		hTree[i]._hWeight = huffTable[tmpIndex-1u];
		hTree[i]._hParent = 0u;
		hTree[i]._hLeftChild = 0u;
		hTree[i]._hRightChild = 0u;
	}

	/* Init Children Nodes */
	for(uint16_t i = numLeafNode + 1u; i <= numAllNode; i++)
	{
		hTree[i]._hWeight = 0u;
		hTree[i]._hParent = 0u;
		hTree[i]._hLeftChild = 0u;
		hTree[i]._hRightChild = 0u;
	}

	/* Create the Tree */
	for(uint16_t i = numLeafNode + 1u; i <= numAllNode; i++)
	{
		::selectMinNode(hTree, i-1u, minNode1, minNode2);
		hTree[minNode1]._hParent = i;
		hTree[minNode2]._hParent = i;
		hTree[i]._hLeftChild = minNode1;
		hTree[i]._hRightChild = minNode2;
		hTree[i]._hWeight = hTree[minNode1]._hWeight + hTree[minNode2]._hWeight;
	}
}

/**
 * @brief	get the huffman code of every element in code table
 */
void getHuffmanCode(huffNode* hTree, uint16_t numLeafNode, huffCode& cTable)
{
	/* check */
	if(hTree == nullptr || numLeafNode == 0u)
	{
		return ;
	}

	uint8_t* tmp = new uint8_t[numLeafNode - 1u];

	uint16_t current = 0u, parent = 0u;
	uint16_t start = 0u;
	
	for(uint16_t i = 1u; i <= numLeafNode; i++)
	{
		start = numLeafNode - 1u;
		current = i;
		parent = hTree[current]._hParent;

		while(parent)
		{
			/* if the node is left child tree */
			if(current == hTree[parent]._hLeftChild)
			{
				tmp[--start] = '0';
			}
			else
			{
				tmp[--start] = '1';
			}
			current = parent; parent = hTree[parent]._hParent; // go to upper node
		}

		cTable._huffBitsLen[i-1u] = numLeafNode - start - 1u;		
		cTable.assginMem(i-1u, numLeafNode-start-1u);

		/* copy the code */
		memcpy(cTable._huffBits[i-1u], tmp+start, cTable._huffBitsLen[i-1u]);
	}

	delete[] tmp;
}

/**
 * @brief	Encode the origin sequence
 */
void toEncoding(uint8_t* srcArr, uint32_t srcLength, uint8_t* dstCode, uint32_t codeLength, huffCode* hTable)
{
	/* check */
	if(srcArr == nullptr || srcLength == 0u || dstCode == nullptr || codeLength == 0u || hTable == 0u)
	{
		return ;
	}

	uint32_t tmp = 0u;

	for(uint32_t i = 0u; i < srcLength; i++)
	{
		tmp = 0u;
		while(hTable->_huffKey[tmp] != srcArr[i])
		{
			tmp++;
		}
		
		memcpy(dstCode, hTable->_huffBits[tmp], hTable->_huffBitsLen[tmp]);
		dstCode += hTable->_huffBitsLen[tmp];
	}
}

/**
 * @brief	Decode the huffman sequence
 */
void toDecoding(huffNode* hTree, uint8_t* hCode, uint32_t codeLength, uint8_t* huffKeys, uint16_t numLeafNode, uint8_t *dstArr)
{
	/* check */
	if(hTree == nullptr || huffKeys== nullptr || hCode == nullptr || dstArr == nullptr || codeLength == 0u || numLeafNode == 0u)
	{
		return ;
	}

	uint16_t position = 0u, numAllCode = numLeafNode * 2u - 1u, xtmp = 0u;;
	uint32_t tmpIndex = 0u;

	while(codeLength --> 0u)
	{
		/* Traverse all of the huffman Tree */
		for(xtmp = numAllCode; hTree[xtmp]._hLeftChild && hTree[xtmp]._hRightChild; position++)
		{
			if(hCode[position] == '0')
			{
				xtmp = hTree[xtmp]._hLeftChild;
			}
			else
			{
				xtmp =  hTree[xtmp]._hRightChild;
			}
		}

		/* Get Origin Sequence */
		dstArr[tmpIndex] = huffKeys[xtmp-1u]; tmpIndex++;
	}
}

/**
 * @brief	Static Huffman Encoding
 * @parma	uint8_t* srcArr
 * 				origin Array to encode
 * @param	uint32_t srcLength
 * 				Length of origin Array
 * @param	sHuffman* xhuff
 * 				Huffman Class to store infomation
 * @return	bool
 * @note	None
 */
bool NXZIP::NXZ_sHuffmanEncode(uint8_t* srcArr, uint32_t srcLength, sHuffman* xhuff)
{
	if(srcArr == nullptr || srcLength == 0u || xhuff == nullptr)
	{
		return false;
	}

	uint16_t nKinds = 0u;
	uint32_t* tmpptr = xhuff->elemFreq;

	::scanOriginSequence(srcArr, srcLength, nKinds, xhuff->elemFreq);

	huffNode newTree[nKinds * 2u] = {0u};
	huffCode* newCode= new huffCode(nKinds);

	/* build code Table */
	for(uint16_t i = 0; i < newCode->_huffNum; i++)
	{
		while(*tmpptr++ == 0u);
		newCode->_huffKey[i] = tmpptr - xhuff->elemFreq - 1u;
	}

	::createHuffmanTree(newTree, nKinds, xhuff->elemFreq);

	::getHuffmanCode(newTree, nKinds, *newCode);

	/* Compute the Code Length(total) */
	uint16_t tmp = 0;
	for(uint16_t i = 1u; i <= newCode->_huffNum; i++)
	{
		while(tmp++ != newCode->_huffKey[i-1u]);
		xhuff->huffCodeLen += newCode->_huffBitsLen[i-1u] * xhuff->elemFreq[tmp-1u];
	}

	xhuff->allocateCodePtr();

	/* Encoding */
	::toEncoding(srcArr, srcLength, xhuff->huffCode, xhuff->huffCodeLen, newCode);

	return true;
}

/**
 * @brief	Static Huffman Decoding
 */
// TODO: Re-write this function
bool NXZIP::NXZ_sHuffmanDecode(uint8_t* dstArr, uint32_t dstLength, sHuffman* zhuff)
{
	if(dstArr == nullptr || dstLength == 0U || zhuff == nullptr)
	{
		return false;
	}

	uint16_t numLeafNode = 0u;
	uint32_t* tmpptr = zhuff->elemFreq;

	/* Statistic for number of Leaf Node */
	for(uint16_t i = 0u; i < 0xFFu; i++)
	{
		if(zhuff->elemFreq[i] != 0)
		{
			numLeafNode++;
		}
	}

	huffNode newTree[numLeafNode * 2u] = {0u};
	uint8_t* __hKeyList = new uint8_t[numLeafNode];

	/* rebuild origin Table */
	for(uint16_t i = 0; i < numLeafNode; i++)
	{
		while(*tmpptr++ == 0u);
		__hKeyList[i] = tmpptr - zhuff->elemFreq - 1u;
	}

	/* rebuild huffman Tree */
	::createHuffmanTree(newTree, numLeafNode, zhuff->elemFreq);

	/* decode */
	::toDecoding(newTree, zhuff->huffCode, zhuff->huffCodeLen, __hKeyList, numLeafNode, dstArr);

	delete[] __hKeyList;

	return true;
}

/**
 * @brief	Write the resulting byte stream to the cache bit by bit(no RLE)
 * @note	will be discarded
 */
bool NXZIP::NXZ_Huffman_RuduceByte2Bit(uint8_t* xCode, uint32_t codeLength, uint8_t*& nBuff, uint32_t& buffSize)
{
	/* Parameters Check */
	if(xCode == nullptr || codeLength == 0u)
	{
		return false;
	}

	/* calculate the buffer size */
	buffSize = codeLength / __CHAR_BIT__ + codeLength % __CHAR_BIT__;

	/* allocate memeory */
	nBuff = new uint8_t[buffSize]{0u};

	/* write into buffer */
	for(uint32_t i = 0u; i < codeLength; i++)
	{
		nBuff[i/__CHAR_BIT__] |= (xCode[i] == '0' ? 0u : 1u) << i % __CHAR_BIT__;
	}

	return true;
}

/**
 * @brief	Extend the read bit stream to a byte stream(no RLE)
 * @note	will be discarded
 */
bool NXZIP::NXZ_Huffman_ExpandBit2Byte(uint8_t* xBytes, uint32_t byteSize, uint8_t*& codeBuff, uint32_t codeLength)
{
	/* parameters check */
	if(xBytes == nullptr || codeLength == 0u || byteSize == 0u)
	{
		return false;
	}

	/* allocate memory */
	codeBuff == new uint8_t[codeLength]{0u};

	/* write into code buffer */
	for(uint32_t i = 0u; i < codeLength; i++)
	{
		codeBuff[i] = xBytes[i/__CHAR_BIT__] & (1u << i % __CHAR_BIT__) ? '1' : '0';
	}

	return true;
}

/**
 * @brief	write the resulting byte stream to cache by RLE, bit by bit
 */
bool NXZIP::NXZ_Huffman_RLE2BitsStream(uint8_t* xCode, uint32_t xLength, utility::vlbuff* eBits)
{
	/* parameters check */
	if(xCode == nullptr || eBits == nullptr ||xLength == 0u)
	{
		return false;
	}

	std::vector<uint8_t> tmp;
	uint8_t tmpcount = 0u;
	uint8_t* tmpCode = xCode;

	while(tmpCode - xCode < xLength)
	{
		if(*tmpCode == '0') { tmp.push_back(0u); }
		if(*tmpCode == '1') { tmp.push_back(1u); }

		/* statistic for element */
		while(*tmpCode == tmp.back() + 48u)
		{
			tmpcount++; tmpCode++;
			if(tmpcount >= 127u) { break; }
		}

		/* encode */
		tmp.back() = tmp.back() << 7u | tmpcount;
		tmpcount = 0u;
	}

	/* take out the result */
	eBits->allocate(tmp.size());
	std::copy(tmp.begin(), tmp.end(), eBits->uptr);

	return true;
}

/**
 * @brief	Decode the bitstream written to the file
 */
bool NXZIP::NXZ_Huffman_RLD2ByteStream(uint8_t* xBytes, uint32_t xLength, utility::vlbuff* yBits)
{
	/* parameters check */
	if(xBytes == nullptr || xLength == 0u || yBits == nullptr)
	{
		return false;
	}

	std::vector<uint8_t> xtmp;
	uint8_t* tmpBytes = xBytes;
	uint8_t tmp = 0u, tmpcount = 0u;

	/* Decode */
	while(tmpBytes - xBytes < xLength)
	{
		tmp = (*tmpBytes & 0x80u >> 7u) + 48u;
		tmpcount = *tmpBytes & 0x7Fu;

		for(uint8_t i = 0u; i < tmpcount; i++)
		{
			xtmp.push_back(tmp);
		}

		tmpBytes++;
	}

	/* take out the result */
	yBits->allocate(xtmp.size());
	std::copy(xtmp.begin(), xtmp.end(), yBits->uptr);

	return true;
}

/* End of File */
