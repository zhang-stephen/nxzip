/**
 * A New Implementation of Huffman Algorithm 
 * 2019-05-15
 */

#include "huffman.h"
#include "zlibh.h"

#define z_NCOUNTBOUND 512
#define z_BLOCKBOUND(size) (size + (size>>7))
#define z_COMPRESSBOUND(size) (z_NCOUNTBOUND + z_BLOCKBOUND(size))   /* Macro version, useful for static allocation */

static size_t CompressBound(size_t size) { return z_COMPRESSBOUND(size); }

/**
 * @brief	Huffman Encoding by Zlib
 */
bool NXZIP::NXZ_Huffman_Encoding(uvbuff* inbuff, uvbuff* outbuff)
{
	/* parameters check */
	if(inbuff->empty() || inbuff->is_null()) { return false; }

	/* Allocate the outbuff */
	outbuff->allocate(CompressBound(inbuff->ulength));

	/* Huffman Encoding */
	outbuff->ulength = (uint32_t)ZLIBH_compress((char*)outbuff->uptr, (char*)inbuff->uptr, (size_t)inbuff->ulength);

	return true;
}

/**
 * @brief	Huffman Decoding by Zlib
 */
bool NXZIP::NXZ_Huffman_Decoding(uvbuff* inbuff, uvbuff* outbuff)
{
	/* parameters check */
	if(inbuff->empty() || inbuff->is_null()) { return false; }

	/* Allocate the outbuff */
	if(!outbuff->is_null()) { outbuff->allocate(); }

	/* Huffman Decoding */
	uint32_t sizecheck = (uint32_t)ZLIBH_decompress((char*)outbuff->uptr, (const char*)inbuff->uptr);

	if(sizecheck != outbuff->ulength) { outbuff->flush(); return false; }

	return true;
}

/* End of File */
