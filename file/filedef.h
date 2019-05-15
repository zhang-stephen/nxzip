/**
 * The Compressed File Section Definitions(header file only)
 * The Expansion of filename is 'zipx'
 * 2019-03-24
 */ 

#ifndef __FILEDEF_H
#define __FILEDEF_H

#include <string>
#include "../crc32/crc32.h"

#ifndef __sizeof_kilobytes 
	#define __sizeof_kilobytes 1024u		// 1KiB = 1024Bytes
#endif /*__sizeof_kilobytes*/

namespace NXZIP
{
	class _header
	{
	public:
		/* Basic Infomation */
		uint8_t zipxID[5];							/* !< File ID, should keep value as "NXZIP" */

		/* Version of Compress Algorithm */
		uint8_t zipxAlgorithmVerSize;
		uint8_t* zipxAlgorithmVer;

		/* Encoding Method */
		uint8_t zipxEncodingLetter;					/* !< Encoding Method, S for Static Huffman Encoding
														A for Arithmetic Encoding, H for Adaptive Huffman Encoding */

		uint32_t zipxCountDataBlocks;				/* !< the counter of data blocks */

		/* Other infomation */
		uint8_t zipxOtherInfoLen;
		uint8_t* zipxOtherInfo;

		_header()
		{
			zipxID[0] = 'N'; zipxID[1] = 'X'; zipxID[2] = 'Z'; zipxID[3] = 'I'; zipxID[4] = 'P';
			zipxAlgorithmVer = nullptr;
			zipxOtherInfo = nullptr;
		}

		void wr_algover(const std::string& ver)
		{
			zipxAlgorithmVer = (uint8_t*)ver.c_str();
			zipxAlgorithmVerSize = ver.size();
		}

		void wr_otherinfo(const std::string& info)
		{
			zipxOtherInfo = (uint8_t*)info.c_str();
			zipxOtherInfoLen = info.size();
		}
	};

	class _datablock
	{
	public:
		uint32_t zipxOriginDataCRC32C;				/* !< crc-32c Value of Origin Data */
		uint32_t zipxBWTBlockSize;					/* !< Block size in BWT */
		uint32_t zipxBWTIndex;						/* !< BWT Index(for inverse-BWT) */

		uint32_t zipxHuffmanCodeSize[2];			/* !< size of Huffman Code cache */
		uint8_t* zipxHuffmanCode;					/* !< Huffman Code Cache*/
	};
}

#endif /*__FILEDEF_H*/

/* End of File */
