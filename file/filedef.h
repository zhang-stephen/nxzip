/**
 * The Compressed File Section Definitions(header file only)
 * The Expansion of filename is 'zipx'
 * 2019-03-24
 */ 

#ifndef __FILEDEF_H
#define __FILEDEF_H

#include <cstdint>

namespace NXZIP
{
	class __zipx_header
	{
	public:
		/* Basic Infomation */
		uint8_t zipxID[5];							/* !< File ID, should keep value as "NXZIP" */
		uint8_t zipxOriginFileNameLen;				/* !< size of Origin Filename */
		uint8_t* zipxOriginFileName;				/* !< Origin Filename, maximum size is 256 byte */
		uint32_t zipxOriginFileNameCRC32C;			/* !< crc-32c Value of Origin Filename */

		/* Version of Compress Version */
		uint8_t zipxCompAlgorMainVer[4];
		uint8_t zipxCompAlgorMidVer[4];
		uint8_t zipxCompAlgorSubVer[4];

		/* General Infomation Block */
		struct 
		{
			uint8_t _zipx_reserved : 6u;			/* !< 4 bits in high is reserved */
			uint8_t _zipx_isParallel : 1u;			/* !< parallel or not */
			uint8_t _zipx_isRLE : 1u;				/* !< used RLE or not */
		}zipxGeneralInfo;							/* !< Store Infomation bit by bit */

		/* Other infomation */
		uint32_t zipxOtherInfoLen;
		char* zipxOtherInfo;

		void fillZipxID(void);
		void processOriginFileName(uint8_t* _filename, uint8_t _len);
		void createVersionString(const uint8_t* _ver);
		
		~__zipx_header(void);
	};
	
	class __zipx_datablock
	{
	public:
		uint32_t* zipxNumberOfBlock;				/* !< Parallel task number for this block(optional) */
		uint32_t zipxOriginDataCRC32C;				/* !< crc-32c Value of Origin Data */
		uint32_t zipxBWTBlockSize;					/* !< Block size in BWT */
		uint32_t zipxBWTIndex;						/* !< BWT Index(for inverse-BWT) */
		
		uint32_t zipxHuffmanBitsLength;				/* !< huffman code length of bit stream */
		uint8_t* zipxHuffmanBits;					/* !< pointer of huffman code */

		uint32_t copyHuffmanBits(uint8_t* _hBits, uint32_t _hBitsLen);

		~__zipx_datablock(void);
	};
}

#endif /*__FILEDEF_H*/

/* End of File */
