/**
 * File Operation Functions(Source file)
 * 2019-04-10
 */

#include <cstdio>
#include <algorithm>
#include <filesystem>

#include "../nxzip.h"
#include "iofile.h"

namespace fs = std::filesystem;
namespace nxzutil = NXZIP::utility;

/* the Version of NXZIP, constant string */
const std::string NXZ_Version = "0.01-alpha";

namespace 
{
	/* a helper class of c-style fileIO */
	/* only support the unix-like fileIO flags, not include the 'b' flag */
	class _io_file
	{
	private:
		std::FILE* _p_fp;
		std::string _p_rw;
		bool _p_fflag;					/* !< true means flush the out */

	public:
		/**
		 * @brief	Default Construction
		 */
		_io_file() { _p_fp = nullptr; }

		/**
		 * @brief	Construction
		 */
		_io_file(const char* fn, const char* rw, bool fflag = false)
		{
			std::string _rw = rw;
		#if WINNT || WIN32 || WIN64
			_rw += "b";
		#endif
			_p_fp = std::fopen(fn, _rw.c_str());
			_p_rw = _rw;
			_p_fflag = fflag;
		}

		/**
		 * @brief Deconstruction
		 */
		~_io_file()
		{
			close();
		}

		/**
		 * @brief 	Open or reopen a file
		 */
		int8_t open(const char* fn, const char* rw, bool fflag = false)
		{
			if(_p_fp != nullptr) 
			{
				if(std::fclose(_p_fp) == EOF) { return -1; }
			}

			std::string _rw = rw;
		#if WINNT || WIN32 || WIN64
			_rw += "b";
		#endif
			_p_fp = std::fopen(fn, _rw.c_str());
			_p_rw = _rw;
			_p_fflag = fflag;
			return (_p_fp == nullptr ? 1u : 0u);
		}

		/**
		 * @brief	write file via binary data(reload 1)
		 */
		int8_t write(uint8_t* wbuff, uint32_t wbsize)
		{
			if(_p_fp == nullptr) { return -1; }
			if(_p_rw.find("w") == std::string::npos && _p_rw.find("+") == std::string::npos) { return -2; }

			if(wbsize != std::fwrite((const void*)wbuff, sizeof(uint8_t), wbsize, _p_fp)) { return -3; }
			else 
			{
				if(_p_fflag)
				{ 
					if(std::fflush(_p_fp) == EOF) { return 1u; }
				}
				return 0u; 
			}
		}

		/**
		 * @brief	write file via binary data(reload 2)
		 */
		int8_t write(nxzutil::VLBUFF& vbuff)
		{
			if(_p_fp == nullptr) { return -1; }
			if(_p_rw.find("w") == std::string::npos && _p_rw.find("+") == std::string::npos) { return -2; }
			if(vbuff.ulength == 0u) { return 0u; }

			if(vbuff.ulength != std::fwrite((const void*)vbuff.uptr, sizeof(uint8_t), vbuff.ulength, _p_fp)) { return -3; }
			else 
			{
				if(_p_fflag)
				{ 
					if(std::fflush(_p_fp) == EOF) { return 1u; }
				}
				return 0u; 
			}
		}

		/**
		 * @brief	read file via binary data(reload 1)
		 */
		int8_t read(nxzutil::VLBUFF& rbuff, uint32_t rbsize)
		{
			if(_p_fp == nullptr) { return -1; }
			if(_p_rw.find("r") == std::string::npos && _p_rw.find("+") == std::string::npos) { return -2; }

			rbuff.allocate(rbsize);

			uint32_t _s_rbsize =  std::fread((void*)rbuff.uptr, sizeof(uint8_t), rbsize, _p_fp);
			if(rbsize > _s_rbsize)
			{
				if(std::feof(_p_fp) != 0u) { return 1u; }
				else { rbuff.ulength = _s_rbsize; return 0u; }
			}
			else
			{
				return 0u;
			}
		}

		/**
		 * @brief	read file via binary data(reload 2)
		 */
		int32_t read(uint8_t* rbuff, uint32_t rbsize)
		{
			if(_p_fp == nullptr || rbuff == nullptr) { return -1; }
			if(_p_rw.find("r") == std::string::npos && _p_rw.find("+") == std::string::npos) { return -2; }

			uint32_t _s_rbsize =  std::fread((void*)rbuff, sizeof(uint8_t), rbsize, _p_fp);
			if(rbsize > _s_rbsize)
			{
				if(std::feof(_p_fp) != 0u) { return 1u; }
				else { return _s_rbsize; }
			}
			return 0u;
		}

		/**
		 * @brief	close opened file
		 */
		inline void close(void)
		{
			if(_p_fp != nullptr) { std::fclose(_p_fp); }
			_p_fp = nullptr;
			_p_rw.clear();
			_p_fflag = false;
		}

		/**
		 * @brief	Query file status
		 */
		bool is_open(void) { return _p_fp == nullptr ? false : true; }

		/**
		 * @brief	Enable/Disable Stream Flush
		 */
		void set_flush(bool flag) { _p_fflag = flag; }

		/**
		 * @brief	Get Real File Pointer
		 */
		std::FILE* get_fp(void) { return _p_fp; }

		/**
		 * @brief	Set the Position of FILE Pointer
		 */
		int seek(uint32_t offset, int origin = SEEK_CUR) { return std::fseek(_p_fp, offset, origin); }
	};

	/* filesystem helper functions */
	/**
	 * @brief	query file exists or not
	 */
	inline bool is_file_exists_fs(const std::string fn)
	{
		return fs::exists(fs::path(fn));
	}

	/**
	 * @brief	query filename is regular file or not
	 */
	inline bool is_regular_file_fs(const std::string fn)
	{
		return fs::is_regular_file(fs::path(fn));
	}

	/**
	 * @brief	query filename is folder or not
	 */
	inline bool is_directory_fs(const std::string fn)
	{
		return fs::is_directory(fs::path(fn));
	}
}

/**
 * @brief	Compress Processing
 */
void NXZIP::NXZ_Compress(std::string& ifile, NXZIP::utility::CLIOPS& ops, std::string& comment, std::string& ofile, uint8_t rblevel)
{
	/* parameters check */
	if(ifile.empty()) { NXZ_PRINT("[ERROR]->filename ERROR\n"); return ; }

	/* BWT Block Size should be [100kiB, 900kiB] */
	if(rblevel < 1u || rblevel > 9u) { NXZ_PRINT("[ERROR]->BWT Block Size ERROR: %d\n", rblevel); return ; }

	/* Input File Check */
	if(::is_file_exists_fs(ifile) == false) { NXZ_PRINT("[ERROR]->The specified file could not be found\n"); return ; }
	else
	{
		if(is_regular_file_fs(ifile) == false) { NXZ_PRINT("[ERROR]->File Type Error\n"); return ; }
	}

	/* Output File Check and filename processing */
	if(::is_file_exists_fs(ofile))
	{
		if(!::is_regular_file_fs(ofile))
		{
			if(::is_directory_fs(ofile)) { ofile = (fs::path(ofile) / fs::path(ifile).filename()).string() + ".xzp"; }
			else { NXZ_PRINT("[ERROR]->File Type Error"); return ; }
		}
	}

	if(!ofile.empty())
	{
		if(ofile.substr(ofile.size() - 4u) != ".xzp") { ofile += ".xzp"; }
	}
	else
	{
		ofile = ifile + ".xzp";
	}

	/* input file size */
	uint32_t tmpfsize = fs::file_size(fs::path(ifile));
	uint32_t rbuffsize = rblevel * 100u * __sizeof_kilobytes;

	/* Temporary String Class */
	std::string tmp = fs::path(ifile).filename().string();		// only took the filename, not full path

	/* try to open files */
	::_io_file I_fp(ifile.c_str(), "r"),
				O_fp(ofile.c_str(), "w", true);
	if(!I_fp.is_open()) { NXZ_PRINT("[ERROR]->No Permission to Read File\n"); return ; }
	if(!O_fp.is_open()) { NXZ_PRINT("[ERROR]->No Permission to Write File\n"); return ; }

	/* Prepare the xzp file header */
	nxz_header fheader;
	fheader.wr_filename(tmp.c_str(), (const uint8_t)tmp.size());
	fheader.wr_algover(NXZ_Version);
	fheader.wr_otherinfo(comment);

	fheader.zipxEncodingLetter = 'S';
	fheader.zipxCountDataBlocks = tmpfsize / rbuffsize + (tmpfsize % rbuffsize == 0u ? 0u : 1u);

	nxzutil::VLBUFF mheader_buff;
	mheader_buff.allocate(5u + 1u + fheader.zipxOriginFileNameLen + 4u + 
							1u + fheader.zipxAlgorithmVerSize + 
							1u + 4u + 
							1u + fheader.zipxOtherInfoLen);
	mheader_buff.vlcopy((void*)fheader.zipxID, 5u);
	mheader_buff.vlcopy((void*)&fheader.zipxOriginFileNameLen, 1u);
	mheader_buff.vlcopy((void*)fheader.zipxOriginFileName, fheader.zipxOriginFileNameLen);
	mheader_buff.vlcopy((void*)&fheader.zipxOriginFileNameCRC32C, sizeof(uint32_t));
	mheader_buff.vlcopy((void*)&fheader.zipxAlgorithmVerSize, sizeof(uint8_t));
	mheader_buff.vlcopy((void*)fheader.zipxAlgorithmVer, fheader.zipxAlgorithmVerSize);
	mheader_buff.vlcopy((void*)&fheader.zipxEncodingLetter, sizeof(uint8_t));
	mheader_buff.vlcopy((void*)&fheader.zipxCountDataBlocks, sizeof(uint32_t));
	mheader_buff.vlcopy((void*)&fheader.zipxOtherInfoLen, sizeof(uint8_t));
	mheader_buff.vlcopy((void*)fheader.zipxOtherInfo, fheader.zipxOtherInfoLen);

	/* write header into xzp file */
	if(O_fp.write(mheader_buff.uptr, mheader_buff.ulength) != 0u) { NXZ_PRINT("[ERROR]->Write xzp File Header Failed!"); return ; }

	/* Prepare to compress data block */
	nxzutil::VLBUFF _t_rbuff;

	/* write data block into xzp file */
	for(uint32_t i = 0u; i < fheader.zipxCountDataBlocks; i++)
	{
		uint32_t tmprbsize = (i == fheader.zipxCountDataBlocks - 1u && tmpfsize % rbuffsize != 0u) ? 
								tmpfsize - rbuffsize * i : 
								rbuffsize;
		
		/* read file to cache */
		if(I_fp.read(_t_rbuff, tmprbsize) < 0u) { NXZ_PRINT("[ERROR]->Block: %d, file read Failed!\n"); return ; }

		/* Burrows-Wheeler Transform */
		BWT _t_bwt(tmprbsize);
		if(!NXZ_BWTransform2(_t_rbuff.uptr, _t_rbuff.ulength, &_t_bwt)) { NXZ_PRINT("[ERROR]->Block: %d, BWT Failed!\n", i+1u); return ; }

		/* Move-To Front Transform */
		nxzutil::VLBUFF _t_mtf(_t_bwt.length);
		if(!NXZ_MoveToFront(_t_bwt.cstr, _t_bwt.length, _t_mtf.uptr)) { NXZ_PRINT("[ERROR]->Block: %d, MTF Failed!\n", i+1u); return ; }

		/* Run-Length Encoding */
		nxzutil::VLBUFF _t_rlc;
		if(NXZ_mRunLength_Encoding(_t_mtf.uptr, _t_mtf.ulength, &_t_rlc) == false) { NXZ_PRINT("[ERROR]->Block: %d, RLE Failed!\n", i+1u); return ; }

		/* Huffman Encoding */
		sHuffman _t_huff{{0u}, 0u, nullptr};
		nxzutil::VLBUFF _t_hbits;
		if(!NXZ_sHuffmanEncode(_t_rlc.uptr, _t_rlc.ulength, &_t_huff)) { NXZ_PRINT("[ERROR]->Block: %d, Huffman Encoding Failed!\n", i+1u); return ; }
		if(!NXZ_Huffman_RuduceByte2Bit(_t_huff.huffCode, _t_huff.huffCodeLen, &_t_hbits)) { NXZ_PRINT("[ERROR]->Block: %d, Bit Stream Failed!\n", i+1u); return ; }

		/* prepare the data block */
		uint32_t _db_crc32 = NXZ_CRC32_Calculate(0u, _t_rbuff.uptr, _t_rbuff.ulength);
		nxzutil::VLBUFF _m_db_cache(sizeof(uint32_t) * (1u + 1u + 1u + 256u + 1u) + _t_hbits.ulength);
		_m_db_cache.vlcopy((void*)&_db_crc32, sizeof(uint32_t));
		_m_db_cache.vlcopy((void*)&_t_bwt.length, sizeof(uint32_t));
		_m_db_cache.vlcopy((void*)&_t_bwt.index, sizeof(uint32_t));
		_m_db_cache.vlcopy((void*)_t_huff.elemFreq, sizeof(uint32_t) * 256u);
		_m_db_cache.vlcopy((void*)&_t_huff.huffCodeLen, sizeof(uint32_t));
		_m_db_cache.vlcopy((void*)_t_hbits.uptr, _t_hbits.ulength);

		/* write into file */
		if(O_fp.write(_m_db_cache.uptr, _m_db_cache.ulength) != 0u) { NXZ_PRINT("[ERROR]->Write Data Block %d Failed!", i); return ; }
	}

	/* close files */
	I_fp.close();
	O_fp.close();

	/* Try to delete input file */
	if(ops.isRemoveFile) { std::remove(ifile.c_str()); }
	if(!fs::exists(fs::path(ifile))) { NXZ_PRINT("[WARNING]->Compress Complete, Delete Failed!"); return ; }
}

/**
 * @brief	Decompress the compressed file
 */
void NXZIP::NXZ_Decompress(std::string& ifile)
{
	/* Parameter check */
	if(ifile.empty()) { NXZ_PRINT("[ERROR]->Filename Empty!"); return ; }

	/* file check */
	if(!fs::exists(fs::path(ifile))) { NXZ_PRINT("Compressed File not Exist!"); return ; }

	/* try to open compressed file */
	::_io_file I_fp(ifile.c_str(), "r");
	if(!I_fp.is_open()) { NXZ_PRINT("[ERROR]->No Permission to Read File"); return ; }

	/* for file read error handling */
	bool _err_f = false; /* general flag of error */
ERR_READFAIL:
	if(_err_f) { NXZ_PRINT("[ERROR]->File Read Failed, unknown reason!"); _err_f = false; return ; }

	/* read file header */
	uint8_t _id[5] = {0u};
	if(I_fp.read(_id, 5u) != 0u) { _err_f = true; goto ERR_READFAIL; }
	if(std::memcmp((const void*)_id, (const void*)"NXZIP", 5u) != 0u) { NXZ_PRINT("[ERROR]->Unexpected File Hader!"); return ; }

	uint8_t _ofnl = 0u;		/* origin file name length */
	if(I_fp.read(&_ofnl, 1u) != 0u) { _err_f = true; goto ERR_READFAIL; };

	std::string _ofn;		/* origin file name */
	if(I_fp.read((uint8_t*)&_ofn[0u], _ofnl) != 0u) { _err_f = true; goto ERR_READFAIL; }

	uint32_t _ofncrc32 = 0u; /* crc-32c value of origin file name */
	if(I_fp.read((uint8_t*)&_ofncrc32, sizeof(uint32_t)) != 0u) { _err_f = true; goto ERR_READFAIL; }
	if(_ofncrc32 != NXZ_CRC32_Calculate(0u, (uint8_t*)&_ofn[0u], _ofnl)) { NXZ_PRINT("[ERROR]->FILENAME crc-32c check failed!"); return ; }

	uint8_t _versize = 0u; /* Algorithm String Length */
	if(I_fp.read(&_versize, 1u) != 0u) { _err_f = true; goto ERR_READFAIL; }

	std::string _ver;		/* Algorithm String */
	if(I_fp.read((uint8_t*)&_ver[0u], _versize) != 0u) { _err_f = true; goto ERR_READFAIL; }
	if(std::memcmp((const void*)NXZ_Version.c_str(), (const void*)_ver.c_str(), _versize) == 1u) { NXZ_PRINT("[WARNING]->Current Edition is Lower than that Generated FILE! Try to Decompress!"); }

	uint8_t _encode = 0u; /* Encode method */
	if(I_fp.read(&_encode, 1u) != 0u) { _err_f = true; goto ERR_READFAIL; }

	uint32_t _db_count = 0u; /* Counts of Data Blocks */
	if(I_fp.read((uint8_t*)&_db_count, sizeof(uint32_t)) != 0u) { _err_f = true; goto ERR_READFAIL; }

	uint8_t _cmml = 0u; 	/* Comment length */
	if(I_fp.read(&_cmml, 1u) != 0u) { _err_f = true; goto ERR_READFAIL; }
	if(I_fp.seek(_cmml) != 0u) { NXZ_PRINT("[ERROR]->FILE Pointer ERROR!"); return ; }

	/* try to open outfile */
	// FIXME: ofile string is error!
	 std::string ofile = (fs::current_path() / fs::path(_ofn)).string();
	// std::string ofile = _ofn;
	::_io_file O_fp(ofile.c_str(), "w", true);
	if(!O_fp.is_open()) {NXZ_PRINT("[ERROR]->No Permission to Write this Directory"); return ; }

	/* Create Temporary Variables for Decompressing */
	std::vector<uint32_t> _g_db_info(1u + 1u + 1u + 256u + 1u, 0u);

	/* read data block and decompress */
	for(uint32_t i = 0u; i < _db_count; i++)
	{
		/* read datablock info */
		if(I_fp.read((uint8_t*)&_g_db_info[0u], _g_db_info.size()) != 0u) { NXZ_PRINT("[ERROR]->Read Datablock: %d Failed, Compressed File is Corrupt!", i); return ; }

		/* read Byte Stream */
		utility::VLBUFF _x_rbuff(_g_db_info.back() / __CHAR_BIT__ + (_g_db_info.back() % __CHAR_BIT__ == 0u ? 0u : 1u));
		if(I_fp.read(_x_rbuff, _x_rbuff.ulength) != 0u) { NXZ_PRINT("[ERROR]->Read Datablock: %d Failed, Compressed File is Corrupt!", i); return ; }

		utility::VLBUFF _x_tmp, _x_rlc;	/* _x_rlc for RLE before Order-zero code */

		switch(_encode)
		{
		case 'S':
			/* Expand Byte Stream to single Bit Stream */
			_x_rlc.allocate(_g_db_info.back());
			if(!NXZ_Huffman_ExpandBit2Byte(_x_rbuff.uptr, _x_rbuff.ulength, _g_db_info.back(), &_x_tmp)) { NXZ_PRINT("[ERROR]->Huffman Decoding Progress 1 Failed in %d Block", i); return ; }
			if(!NXZ_sHuffmanDecode(_x_rlc.uptr, _x_rlc.ulength, &_g_db_info[3], _x_tmp.uptr, _x_tmp.ulength)) { NXZ_PRINT("[ERROR]->Huffman Decoding Progress 2 Failed in %d Block", i); return ; }
			break ; 
		case 'H':
			/* TODO: Adaptive Huffman Decoding */
			break;
		case 'A':
			/* TODO: Arithmetic Decoding */
			break;
		default:
			NXZ_PRINT("[ERROR]->Unknown Encoding Method");
			return;
		}

		/* Run-Length Decoding */
		utility::VLBUFF _x_mtf;
		if(!NXZ_mRunLength_Decoding(_x_rlc.uptr, _x_rlc.ulength, &_x_mtf)) { NXZ_PRINT("[ERROR]->Run-Length Decoding Failed in %d block", i); return ; }

		/* Inverse Move-To-Front Transform */
		utility::VLBUFF _x_bwt(_g_db_info[2u]);
		if(!NXZ_MoveToFront_Inverse(_x_mtf.uptr, _x_mtf.ulength, _x_bwt.uptr)) { NXZ_PRINT("[ERROR]->Inverse-MTF Failed in %d block", i); return ; }

		/* Inverse Burrows-Wheeler Transform */
		utility::VLBUFF _x_obuff(_g_db_info[2u]);
		if(!NXZ_BWTransform_Inverse2(_x_bwt.uptr, _x_bwt.ulength, _g_db_info[2u], _x_obuff.uptr)) { NXZ_PRINT("[ERROR]->Inverse-BWT Failed in %d block", i); return ; }

		/* CRC-32c for data block */
		if(_g_db_info[0u] != NXZ_CRC32_Calculate(0u, _x_obuff.uptr, _x_obuff.ulength)) { NXZ_PRINT("[WARNING]->CRC Failed in %d block, Compressed File maybe Corrupt", i); }

		/* Write into outfile */
		O_fp.write(_x_obuff);
	}
}

/* End of File */
