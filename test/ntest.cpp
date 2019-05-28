/**
 * A Test Function of NXZIP with CLI-options(source file)
 * 2019-05-28
 */

#include <iostream>

#include "../nxzip.h"
#include "../nxzutil.h"

#include "ntest.h"
#include "file.h"

#ifndef __SIZEOF_TEST_RBUFF
#	define __SIZEOF_TEST_RBUFF 102400 /* 100 kiB */
#endif /*__SIZEOF_TEST_RBUFF*/

/**
 * @brief	Test for Basic Stream of Algorithms
 */
void NXZIP::NXZ_BasicStream_Test(const std::string& ifile)
{
	/* parameters check */
	if(ifile.empty()) { std::cout << "Error: Input filename is empty!" << std::endl; return ; }

	/* file type check */
	if(!Stream::is_regular_file(ifile.c_str())) { std::cout << "Error: Input file type is empty!" << std::endl; return ; }

	/* file exist check */
	if(!Stream::is_file_exists(ifile.c_str())) { std::cout << "Error: Cannot find this file!" << std::endl; return ; }

	/* try to open file */
	Stream::_io_file rfile(ifile.c_str(), "r");
	if(!rfile.is_open()) { std::cout << "Error: No Permission to read file!" << std::endl; return ; }

	/* try to read file */
	uint8_t* rbuff = new uint8_t[__SIZEOF_TEST_RBUFF]{0u};
	int32_t tmp = rfile.read(rbuff, __SIZEOF_TEST_RBUFF);
	uint32_t rbsize = 0u;
	if(tmp < 0) { std::cout << "Error: Read file failed!" << std::endl; delete[] rbuff; return ; }
	else if(tmp == 0u) { rbsize = __SIZEOF_TEST_RBUFF; }
	else { rbsize = tmp; }

	uint8_t* xxbuff1 = new uint8_t[rbsize]{0u};
	uint8_t* xxbuff2 = new uint8_t[rbsize]{0u};

	/* try to process it */
	/* Burrows-Wheeler Transform */
	BWT mybwt(rbsize);
	NXZ_BWTransform2(rbuff, rbsize, &mybwt);
	NXZ_BWTransform_Inverse2(mybwt.cstr, mybwt.length, mybwt.index, xxbuff2);

	if(std::memcmp(rbuff, xxbuff2, rbsize) != 0) {std::cout << "Error: BWT Failed!" << std::endl; }
	else {std::cout << "Info: BWT Okay!" << std::endl; }

	std::memset(xxbuff1, 0u, rbsize);
	std::memset(xxbuff2, 0u, rbsize);

	/* Move-To Front */
	NXZ_MoveToFront(rbuff, rbsize, xxbuff1);
	NXZ_MoveToFront_Inverse(xxbuff1, rbsize, xxbuff2);

	if(std::memcmp(rbuff, xxbuff2, rbsize) != 0) {std::cout << "Error: MTF Failed!" << std::endl; }
	else {std::cout << "Info: MTF Okay!" << std::endl; }

	std::memset(xxbuff1, 0u, rbsize);
	std::memset(xxbuff2, 0u, rbsize);

	/* RLE-0 */
	utility::VLBUFF rle, rld;
	NXZ_mRunLength_Encoding(rbuff, rbsize, &rle);
	NXZ_mRunLength_Decoding(rle.uptr, rle.ulength, &rld);

	if(std::memcmp(rbuff, rld.uptr, rbsize) != 0) {std::cout << "Error: RLE-0 Failed!" << std::endl; }
	else {std::cout << "Info: RLE-0 Okay!" << std::endl; }

	/* Canonical Huffman */
	utility::VLBUFF huff1, huff2(rbsize), inbuff(rbsize);
	inbuff.vlcopy(rbuff, rbsize);
	NXZ_Huffman_Encoding(&inbuff, &huff1);
	NXZ_Huffman_Decoding(&huff1, &huff2);

	if(std::memcmp(rbuff, huff2.uptr, rbsize) != 0) {std::cout << "Error: Huffman Failed" << std::endl; }
	else { std::cout << "Info: Huffman Okay!" << std::endl; }
	
	delete[] xxbuff1;
	delete[] xxbuff2;
}

