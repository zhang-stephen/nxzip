/**
 * The Compressed File Section Definitions(header file only)
 * The Expansion of filename is 'zipx'
 * 2019-03-24
 */ 

#ifndef __FILEDEF_H
#define __FILEDEF_H

#include <cstdint>

#ifndef __sizeof_kilobytes 
	#define __sizeof_kilobytes 1024u		// 1KiB = 1024Bytes
#endif /*__sizeof_kilobytes*/

namespace NXZIP
{
	struct nxz_header
	{
		/* Basic Infomation */
		uint8_t zipxID[5];							/* !< File ID, should keep value as "NXZIP" */
		uint8_t zipxOriginFileNameLen;				/* !< size of Origin Filename */
		uint8_t* zipxOriginFileName;				/* !< Origin Filename, maximum size is 256 byte */
		uint32_t zipxOriginFileNameCRC32C;			/* !< crc-32c Value of Origin Filename */

		/* Version of Compress Algorithm */
		uint8_t zipxAlgorithmVerSize;
		uint8_t* zipxAlgorithmVer;

		/* General Infomation Block */
		struct 
		{
			uint8_t _zipx_reserved : 5u;			/* !< 4 bits in high is reserved */
			uint8_t _zipx_encoding : 2u;			/* !< encoding type: 00->static huffman, 01->canonical huffman
														10->arthmetic encoding, 11->reserved */
			uint8_t _zipx_isParallel : 1u;			/* !< parallel or not */
		}zipxGeneralInfo;							/* !< Store Infomation bit by bit */

		uint32_t zipxCountDataBlocks;					/* !< the counter of data blocks */

		/* Other infomation */
		uint8_t zipxOtherInfoLen;
		uint8_t* zipxOtherInfo;
	};
	
	class nxz_datablock
	{
	public:
		uint32_t zipxOriginDataCRC32C;				/* !< crc-32c Value of Origin Data */
		uint32_t zipxBWTBlockSize;					/* !< Block size in BWT */
		uint32_t zipxBWTIndex;						/* !< BWT Index(for inverse-BWT) */
		
		uint32_t* zipxHuffmanFreq;					/* !< Huffman Probability Model */
		uint32_t zipxHuffmanBitsLength;				/* !< huffman code length of bit stream */
		uint8_t* zipxHuffmanBits;					/* !< pointer of huffman code */
	};
}

#endif /*__FILEDEF_H*/

/* End of File */
