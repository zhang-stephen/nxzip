/**
 * File Operation Functions(header file)
 * 2019-04-10
 */

#ifndef __IO_FILE_H
#define __IO_FILE_H

#include <string>

#include "filedef.h"
#include "../nxzutil.h"

#ifndef NXZ_ENCODING_METHOD
	#define NXZ_ENCODING_METHOD 0u		// Encoding Method: static huffman encoding
#endif /*NXZ_ENCODING_METHOD*/

namespace fs = std::filesystem;

extern const std::string NXZ_Version;

namespace NXZIP
{
	void NXZ_Compress(std::string& iflie, NXZIP::utility::CLIOPS& ops, std::string& comment, std::string& ofile, uint8_t rblevel);
	void NXZ_Decompress(std::string& ifile);
	void NXZ_ShowZipxInfo(std::string& ifile);
}

#endif /*__IO_FILE_H*/

/* End of File */