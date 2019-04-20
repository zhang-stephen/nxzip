/**
 * File Operation Functions(Source file)
 * 2019-04-10
 */

#include <fstream>
#include <algorithm>
#include <filesystem>

#include "iofile.h"

namespace fs = std::filesystem;
namespace nxzutil = NXZIP::utility;

/* the Version of NXZIP, constant string */
const std::string NXZ_Version = "0.01-alpha";

int8_t compress_write_block(std::ifstream& ifp, std::ofstream& ofp, uint32_t rfbuffsize, uint8_t* rbuff);

/**
 * @brief Compress file
 */
void NXZIP::NXZ_Compress(std::string& ifile, 
						utility::CLIOPS& ops, 
						std::string& comment, 
						std::string& ofile, 
						uint32_t rfbuffsize = 100 * __sizeof_kilobytes)
{
	/* parameters check */
	if(ifile.empty() || ofile.empty() || rfbuffsize < 100u*__sizeof_kilobytes || rfbuffsize > 1000u*__sizeof_kilobytes)
	{
		return ;
	}

	/* create temporary variables */
	utility::VLBUFF w_header_cache;			// file header cache;
	nxz_header mheader = {0u};				// file header

	/* verify files  */
	fs::path _i_path(ifile), _o_path(ofile);
	std::string fname = _i_path.filename().string();
	if(fs::exists(_i_path) == false) { return; }
	else
	{
		if(fs::is_regular_file(_i_path) == false) { return ; }
	}

	if(fs::exists(_o_path) == true)
	{
		if(fs::is_regular_file(_o_path) == false) { return ; }
		if(fs::is_directory(_o_path) == true ) 
		{
			if(ofile.back() == '/' || ofile.back() == '\\') { ofile += (fname + ".xzp"); }
			else { ofile += ("/" + fname + ".xzp"); }
		}
	}
	else 
	{ 
		ofile = ifile + ".xzp"; 
	}

	std::ifstream _i_fp(ifile, std::ios::binary | std::ios::in);
	std::ofstream _o_fp(ofile, std::ios::binary | std::ios::trunc | std::ios::out);
	if(_i_fp.is_open() == false) { return ; }	// no permission
	if(_o_fp.is_open() == false) { _i_fp.close(); return ; }	// no permission

	/* write file header */
	mheader.zipxID[0] = 'N';
	mheader.zipxID[1] = 'X';
	mheader.zipxID[2] = 'Z';
	mheader.zipxID[3] = 'I';
	mheader.zipxID[4] = 'P';
	mheader.zipxOriginFileNameLen = fname.size();
	mheader.zipxOriginFileName = (uint8_t*)fname.c_str();
	mheader.zipxOriginFileNameCRC32C = NXZ_CRC32_Calculate(0u, mheader.zipxOriginFileName, mheader.zipxOriginFileNameLen);
	mheader.zipxAlgorithmVerSize = NXZ_Version.size();
	mheader.zipxAlgorithmVer = (uint8_t*)NXZ_Version.c_str();
	mheader.zipxGeneralInfo._zipx_encoding = ops.encoding & 0x03u;
	mheader.zipxCountDataBlocks = fs::file_size(_i_path) / rfbuffsize + (fs::file_size(_i_path) / rfbuffsize == 0 ? 0u : 1u);
	mheader.zipxOtherInfoLen - comment.size();
	mheader.zipxOtherInfo = (uint8_t*)comment.c_str();

	/* write into header cache */
	w_header_cache.allocate(5u + 1u + mheader.zipxOriginFileNameLen + 4u
							+ 1u + mheader.zipxAlgorithmVerSize 
							+ 1u + 4u 
							+ 1u + mheader.zipxOtherInfoLen);
	w_header_cache.vlcopy(mheader.zipxID, 5u);
	w_header_cache.vlcopy(&mheader.zipxOriginFileNameLen, 1u);
	w_header_cache.vlcopy(mheader.zipxOriginFileName, mheader.zipxOriginFileNameLen);
	w_header_cache.vlcopy(&mheader.zipxOriginFileNameCRC32C, 4u);
	w_header_cache.vlcopy(&mheader.zipxAlgorithmVerSize, 1u);
	w_header_cache.vlcopy(mheader.zipxAlgorithmVer, mheader.zipxAlgorithmVerSize);
	w_header_cache.vlcopy(&mheader.zipxGeneralInfo, 1u);
	w_header_cache.vlcopy(&mheader.zipxCountDataBlocks, 4u);
	w_header_cache.vlcopy(&mheader.zipxOtherInfoLen, 1u);
	w_header_cache.vlcopy(mheader.zipxOtherInfo, mheader.zipxOtherInfoLen);

	/* write datablocks into xzp file */
	_o_fp.write((const char*)w_header_cache.uptr, w_header_cache.ulength);
	_o_fp.flush();

	utility::VLBUFF rbuff(rfbuffsize);

	/* write datablocks into xzp file */
	for(uint32_t i = 0u; i < mheader.zipxCountDataBlocks; i++)
	{
		if(i == mheader.zipxCountDataBlocks - 1u)
		{
			if(fs::file_size(_i_path) % rfbuffsize != 0u)
			{
				rfbuffsize = fs::file_size(_i_path) % rfbuffsize;
			}
		}
		if(compress_write_block(_i_fp, _o_fp, rfbuffsize, rbuff.uptr) != 0u) { return ; }
	}

	/* release files */
	_i_fp.close();
	_o_fp.close();

	/* try to delete input file */
	if(ops.isRemoveFile == true) { std::remove(ifile.c_str()); }
}

int8_t compress_write_block(std::ifstream& ifp, std::ofstream& ofp, uint32_t rfbuffsize, uint8_t* rbuff)
{
	/* parameters check */
	if(ifp.is_open() == false || ofp.is_open() == false || rbuff == nullptr)
	{
		return -1;
	}

	if(rfbuffsize == 0u) { return 0u; }

	/* using namespace */
	using namespace NXZIP;

	/* create temporary variables */
	utility::VLBUFF rfbuff(rfbuffsize);
	BWT _i_bwt(rfbuffsize);					// bwt class
	utility::VLBUFF _i_mtf(rfbuffsize);		// mtf buffer
	utility::VLBUFF _i_rlc;					// rlc buffer
	sHuffman _i_huff = {{0u}, 0u, nullptr};	// huffman class
	utility::VLBUFF _b_huff;				// huffman bits to bytes
	nxz_datablock _m_block;					// data block
	utility::VLBUFF m_block_cache;			// data block cache

	ifp.read((char*)rfbuff.uptr, rfbuffsize);

	/* Burrows-Wheeler Transform */
	if(NXZ_BWTransform2(rfbuff.uptr, rfbuffsize, &_i_bwt) == false) { return -1; }

	/* Move-To Front Transform */
	if(NXZ_MoveToFront(_i_bwt.cstr, _i_bwt.length, _i_mtf.uptr) == false) { return -1; }

	/* Run-Length Encoding for MTF */
	if(NXZ_mRunLength_Encoding(_i_mtf.uptr, _i_mtf.ulength, &_i_rlc) == false) { return -1; }

	/* Huffman Encoding */
	if(NXZ_sHuffmanEncode(_i_rlc.uptr, _i_rlc.ulength, &_i_huff) == false) { return -1; }
	if(NXZ_Huffman_RuduceByte2Bit(_i_huff.huffCode, _i_huff.huffCodeLen, &_b_huff) == false) { return -1; }

	// delete[] rfbuff;

	/* write into data block */
	_m_block.zipxOriginDataCRC32C = NXZ_CRC32_Calculate(0u, rfbuff.uptr, rfbuffsize);
	_m_block.zipxBWTBlockSize = rfbuffsize;
	_m_block.zipxBWTIndex = _i_bwt.index;
	_m_block.zipxHuffmanFreq = _i_huff.elemFreq;
	_m_block.zipxHuffmanBitsLength = _i_huff.huffCodeLen;
	_m_block.zipxHuffmanBits = _b_huff.uptr;

	ofp.write((const char*)&_m_block.zipxOriginDataCRC32C, sizeof(uint32_t));
	ofp.write((const char*)&_m_block.zipxBWTBlockSize, sizeof(uint32_t));
	ofp.write((const char*)&_m_block.zipxBWTIndex, sizeof(uint32_t));
	ofp.write((const char*)_m_block.zipxHuffmanFreq, 256u * sizeof(uint32_t));
	ofp.write((const char*)&_m_block.zipxHuffmanBitsLength, sizeof(uint32_t));
	ofp.write((const char*)_m_block.zipxHuffmanBits, _b_huff.ulength);

	ofp.flush();

	return 0u;
}

/* End of File */
