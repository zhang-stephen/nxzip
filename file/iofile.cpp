/**
 * File Operation Functions(Source file)
 * 2019-04-10
 */

#include <cstdio>
#include <iostream>
#include <algorithm>
#include <vector>
#include <sys/stat.h>
#include <fcntl.h>

#include "../nxzip.h"
#include "iofile.h"

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

	/**
	 * @brief	query file exists or not
	 */
	bool is_file_exists(char const* fn)
	{
		std::FILE *tmp = std::fopen(fn, "r");
		bool exists = (tmp != nullptr);
		if (tmp != nullptr) fclose (tmp);
		return exists;
	}

	/**
	 * @brief	query filename is regular file or not
	 */
	bool is_regular_file(char const* fn)
	{
		struct stat buf;
		
		;stat(fn, &buf);
		if(S_ISREG(buf.st_mode) == 0u) { return false; }
		else { return true; }
	}

	/**
	 * @brief	Get File Size
	 */
	int64_t file_size(char const* fn)
	{
		struct stat buf = {0u};

		if(stat(fn, &buf) != 0) { return -1; }
		return buf.st_size;
	}
}

/**
 * @brief	Compress Processing
 * @param	string& ifile
 * 				input file path string
 * @param	CLIOPS& ops
 * 				Command line options
 * @param	string& comment
 * 				Comment of the compressed file
 * @param	string& ofile
 * 				path of compressed file
 * @param	uint8_t rblevel
 * 				BWT Block size(1 for 100kiB, max is 9)
 * @return	None
 * @note 	None
 */
void NXZIP::NXZ_Compress(std::string& ifile, NXZIP::utility::CLIOPS& ops, std::string& comment, uint8_t rblevel)
{
	/* parameters check */
	if(ifile.empty()) { NXZ_PRINT("[ERROR]->filename ERROR\n"); return ; }

	/* file type check */
	if(!::is_regular_file(ifile.c_str())) { NXZ_PRINT("[ERROR]->Input File is not Regular Type: %d", ::is_regular_file(ifile.c_str())); return ; }

	/* BWT Block Size should be [100kiB, 900kiB] */
	if(rblevel < 1u || rblevel > 9u) { NXZ_PRINT("[ERROR]->BWT Block Size ERROR: %d", rblevel); return ; }

	/* Output File Check and filename processing */
	std::string ofile = ifile + ".xcp";

	/* input file size */
	uint32_t tmpfsize = ::file_size(ifile.c_str());
	uint32_t rbuffsize = rblevel * 100u * __sizeof_kilobytes;

	NXZ_TRACE_INFO("Input File Size: %ld", tmpfsize);

	/* try to open files */
	::_io_file I_fp(ifile.c_str(), "r"),
				O_fp(ofile.c_str(), "w");
	if(!I_fp.is_open()) { NXZ_PRINT("[ERROR]->No Permission to Read File"); return ; }
	if(!O_fp.is_open()) { NXZ_PRINT("[ERROR]->No Permission to Write File"); return ; }

	/* Prepare the xzp file header */
	_header fheader;
	fheader.wr_algover(NXZ_Version);
	fheader.wr_otherinfo(comment);

	fheader.zipxEncodingLetter = 'H';
	fheader.zipxCountDataBlocks = tmpfsize / rbuffsize + (tmpfsize % rbuffsize == 0u ? 0u : 1u);

	nxzutil::VLBUFF mheader_buff;
	mheader_buff.allocate(5u + 
						1u + fheader.zipxAlgorithmVerSize + 
						1u + 4u + 
						1u + fheader.zipxOtherInfoLen);
	mheader_buff.vlcopy((void*)fheader.zipxID, 5u);
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
		NXZ_PRINT("Compress is ongoing. Blocks: %d/%d", i+1, fheader.zipxCountDataBlocks);

		uint32_t tmprbsize = (i == fheader.zipxCountDataBlocks - 1u && tmpfsize % rbuffsize != 0u) ? 
								tmpfsize - rbuffsize * i : 
								rbuffsize;
		
		/* read file to cache */
		if(I_fp.read(_t_rbuff, tmprbsize) < 0u) { NXZ_PRINT("[ERROR]->Block: %d, file read Failed"); return ; }

		/* Burrows-Wheeler Transform */
		BWT _t_bwt(tmprbsize);
		if(!NXZ_BWTransform2(_t_rbuff.uptr, _t_rbuff.ulength, &_t_bwt)) { NXZ_PRINT("[ERROR]->Block: %d, BWT Failed", i+1u); return ; }

		/* Move-To Front Transform */
		utility::VLBUFF _t_mtf(_t_bwt.length);
		if(!NXZ_MoveToFront(_t_bwt.cstr, _t_bwt.length, _t_mtf.uptr)) { NXZ_PRINT("[ERROR]->Block: %d, MTF Failed", i+1u); return ; }

		/* Run-Length Encoding */
		utility::VLBUFF _t_rlc;
		if(NXZ_mRunLength_Encoding(_t_mtf.uptr, _t_mtf.ulength, &_t_rlc) == false) { NXZ_PRINT("[ERROR]->Block: %d, RLE Failed", i+1u); return ; }

		utility::VLBUFF _h_code;
		if(!NXZ_Huffman_Encoding(&_t_rlc, &_h_code)) { NXZ_PRINT("[ERROR]->Block: %d, Huffman Encoding Failed", i+1u); return ; }

		/* prepare the data block */
		uint32_t _db_crc32 = NXZ_CRC32_Calculate(0u, _t_rbuff.uptr, _t_rbuff.ulength);
		// nxzutil::VLBUFF _m_db_cache(sizeof(uint32_t) * (1u + 1u + 1u + 256u + 1u) + _t_hbits.ulength);
		utility::VLBUFF _m_db_cache(sizeof(uint32_t) * 5u + _h_code.ulength);
		_m_db_cache.vlcopy((void*)&_db_crc32, sizeof(uint32_t));
		_m_db_cache.vlcopy((void*)&_t_bwt.length, sizeof(uint32_t));
		_m_db_cache.vlcopy((void*)&_t_bwt.index, sizeof(uint32_t));
		_m_db_cache.vlcopy((void*)&_t_rlc.ulength, sizeof(uint32_t));
		_m_db_cache.vlcopy((void*)&_h_code.ulength, sizeof(uint32_t));
		_m_db_cache.vlcopy((void*)_h_code.uptr, _h_code.ulength);

		/* write into file */
		if(O_fp.write(_m_db_cache.uptr, _m_db_cache.ulength) != 0u) { NXZ_PRINT("[ERROR]->Write Data Block %d Failed", i); return ; }
	}

	/* close files */
	I_fp.close();
	O_fp.close();

	/* Try to delete input file */
	if(ops.isRemoveFile) { std::remove(ifile.c_str()); }
	// if(!fs::exists(fs::path(ifile))) { NXZ_PRINT("[WARNING]->Compress Complete, Delete Failed"); return ; }
}

/**
 * @brief	Decompress the compressed file
 */
void NXZIP::NXZ_Decompress(std::string& ifile)
{
	/* Parameter check */
	if(ifile.empty()) { NXZ_PRINT("[ERROR]->Filename Empty"); return ; }

	/* file extension check */
	if(ifile.substr(ifile.size() - 4u) != ".xcp") { NXZ_PRINT("[WARNING]->Compressed File Externsion Error"); }

	/* file check */
	if(!::is_file_exists(ifile.c_str())) { NXZ_PRINT("[ERROR]->Compressed File not Exist"); return ; }

	/* try to open compressed file */
	::_io_file I_fp(ifile.c_str(), "r");
	if(!I_fp.is_open()) { NXZ_PRINT("[ERROR]->No Permission to Read File"); return ; }

	/* for file read error handling */
	bool _err_f = false; /* general flag of error */
ERR_READFAIL:
	if(_err_f) { NXZ_PRINT("[ERROR]->File Read Failed, unknown reason"); _err_f = false; return ; }

	/* read file header */
	uint8_t _id[5] = {0u};
	if(I_fp.read(_id, 5u) != 0u) { _err_f = true; goto ERR_READFAIL; }
	if(std::memcmp((const void*)_id, (const void*)"NXZIP", 5u) != 0u) { NXZ_PRINT("[ERROR]->Unexpected File Hader"); return ; }

	uint8_t _versize = 0u; /* Algorithm String Length */
	if(I_fp.read(&_versize, 1u) != 0u) { _err_f = true; goto ERR_READFAIL; }

	std::string _ver(_versize, '\0');		/* Algorithm String */
	if(I_fp.read((uint8_t*)&_ver[0u], _versize) != 0u) { _err_f = true; goto ERR_READFAIL; }
	if(std::memcmp((const void*)NXZ_Version.c_str(), (const void*)_ver.c_str(), _versize) == 1u) { NXZ_PRINT("[WARNING]->Current Edition is Lower than that Generated FILE! Try to Decompress"); }

	uint8_t _encode = 0u; /* Encode method */
	if(I_fp.read(&_encode, 1u) != 0u) { _err_f = true; goto ERR_READFAIL; }

	uint32_t _db_count = 0u; /* Counts of Data Blocks */
	if(I_fp.read((uint8_t*)&_db_count, sizeof(uint32_t)) != 0u) { _err_f = true; goto ERR_READFAIL; }

	uint8_t _cmml = 0u; 	/* Comment length */
	if(I_fp.read(&_cmml, 1u) != 0u) { _err_f = true; goto ERR_READFAIL; }
	if(I_fp.seek(_cmml) != 0u) { NXZ_PRINT("[ERROR]->FILE Pointer ERROR"); return ; }

	/* try to open outfile */
	std::string ofile(ifile, 0u, ifile.size() - 4u);
	::_io_file O_fp(ofile.c_str(), "w");
	if(!O_fp.is_open()) {NXZ_PRINT("[ERROR]->No Permission to Write this Directory"); return ; }

	/* Create Temporary Variables for Decompressing */
	// std::vector<uint32_t> _g_db_info(1u + 1u + 1u + 1u + 256u, 0u);
	std::vector<uint32_t> _g_db_info(5u, 0u);

	/* read data block and decompress */
	for(uint32_t i = 0u; i < _db_count; i++)
	{
		NXZ_PRINT("Decompress is ongoing. Blocks: %d/%d", i+1, _db_count);

		/* read datablock info */
		if(I_fp.read((uint8_t*)&_g_db_info[0u], _g_db_info.size() * sizeof(uint32_t)) != 0u) { NXZ_PRINT("[ERROR]->Read Datablock: %d Failed, Compressed File is Corrupt", i); return ; }

		// NXZ_TRACE_INFO("[INFO]->Huffman Code Length: %ld bits", _g_db_info[3]);

		/* read Byte Stream */
		utility::VLBUFF _x_rbuff(_g_db_info[4]);
		if(I_fp.read(_x_rbuff, _x_rbuff.ulength) != 0u) { NXZ_PRINT("[ERROR]->Read Datablock: %d Failed, Compressed File is Corrupt", i); return ; }

		//utility::VLBUFF _x_tmp, _x_rlc;	/* _x_rlc for RLE before Order-zero code */
		utility::VLBUFF _x_rlc(_g_db_info[3]);

		/* Huffman Decoding */
		if(!NXZ_Huffman_Decoding(&_x_rbuff, &_x_rlc)) { NXZ_PRINT("[ERROR]->Huffman Decoding Error in %d Block", i); return ; }

		/* Run-Length Decoding */
		utility::VLBUFF _x_mtf;
		if(!NXZ_mRunLength_Decoding(_x_rlc.uptr, _x_rlc.ulength, &_x_mtf)) { NXZ_PRINT("[ERROR]->Run-Length Decoding Failed in %d block", i); return ; }

		/* Inverse Move-To-Front Transform */
		utility::VLBUFF _x_bwt(_g_db_info[1u]);
		if(!NXZ_MoveToFront_Inverse(_x_mtf.uptr, _x_mtf.ulength-1, _x_bwt.uptr)) { NXZ_PRINT("[ERROR]->Inverse-MTF Failed in %d block", i); return ; }

		/* Inverse Burrows-Wheeler Transform */
		utility::VLBUFF _x_obuff(_g_db_info[1u]);
		if(!NXZ_BWTransform_Inverse2(_x_bwt.uptr, _x_bwt.ulength, _g_db_info[2u], _x_obuff.uptr)) { NXZ_PRINT("[ERROR]->Inverse-BWT Failed in %d block", i); return ; }

		/* CRC-32c for data block */
		if(_g_db_info[0u] != NXZ_CRC32_Calculate(0u, _x_obuff.uptr, _x_obuff.ulength)) { NXZ_PRINT("[WARNING]->CRC Failed in %d block, Compressed File maybe Corrupt", i); }

		/* Write into outfile */
		O_fp.write(_x_obuff);
	}

	/* Close Files */
	I_fp.close();
	O_fp.close();
}

/**
 * @brief	Read Compressed File Information
 */
void NXZIP::NXZ_ShowZipxInfo(std::string& ifile)
{
	/* parameters check */
	if(ifile.empty()) { NXZ_PRINT("[ERROR]->File is Corrupt!"); return ; }

	/* read sys information of compressed file */
	struct stat mystat;
	if(stat(ifile.c_str(), &mystat) != 0) { NXZ_PRINT("[ERROR]->Read File Info Failed!"); return ; }

	::_io_file xfile(ifile.c_str(), "r");


	/* for file read error handling */
	bool _err_f = false; /* general flag of error */
xERR_READFAIL:
	if(_err_f) { NXZ_PRINT("[ERROR]->File Read Failed, unknown reason"); _err_f = false; return ; }
	uint8_t _id[5] = {0u};
	if(xfile.read(_id, 5u) != 0u) { _err_f = true; goto xERR_READFAIL; }
	if(std::memcmp((const void*)_id, (const void*)"NXZIP", 5u) != 0u) { NXZ_PRINT("[ERROR]->Unexpected File Hader"); return ; }

	uint8_t _versize = 0u; /* Algorithm String Length */
	if(xfile.read(&_versize, 1u) != 0u) { _err_f = true; goto xERR_READFAIL; }

	std::string _ver(_versize, '\0');		/* Algorithm String */
	if(xfile.read((uint8_t*)&_ver[0u], _versize) != 0u) { _err_f = true; goto xERR_READFAIL; }
	if(std::memcmp((const void*)NXZ_Version.c_str(), (const void*)_ver.c_str(), _versize) == 1u) { NXZ_PRINT("[WARNING]->Current Edition is Lower than that Generated FILE! Try to Decompress"); }

	uint8_t _encode = 0u; /* Encode method */
	if(xfile.read(&_encode, 1u) != 0u) { _err_f = true; goto xERR_READFAIL; }

	uint32_t _db_count = 0u; /* Counts of Data Blocks */
	if(xfile.read((uint8_t*)&_db_count, sizeof(uint32_t)) != 0u) { _err_f = true; goto xERR_READFAIL; }

	uint8_t _cmml = 0u; 	/* Comment length */
	if(xfile.read(&_cmml, 1u) != 0u) { _err_f = true; goto xERR_READFAIL; }

	std::string _cm(_cmml, '\0');	/* Comment */
	if(xfile.read((uint8_t*)&_cm[0], _cmml) != 0u) { _err_f = true; goto xERR_READFAIL; }

	xfile.close();

	/*  */
	std::string _codec = _encode == 'S' ? "Static Huffman Encoding" : "Arithmetic Encoding";

	/* stdout */
	NXZ_PRINT("Compressed File: %s", ifile.c_str());
	NXZ_PRINT("Source File: %s", ifile.substr(0u, ifile.size() -4).c_str());
	NXZ_PRINT("Compressed Size: %d Bytes", mystat.st_size);
	NXZ_PRINT("Block Count: %d", _db_count);
	NXZ_PRINT("Algor Version: %s", _ver.c_str());
	NXZ_PRINT("Encoding: %s", _codec.c_str());
}

/* End of File */
