/**
 * A Namespace for file Operations
 * 2019-05-22
 */

#ifndef __T_FILE_H
#define __T_FILE_H

#include <cstdio>
#include <string>
#include <sys/stat.h>

namespace NXZIP::Stream
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
		 * @brief	read file via binary data(reload 2)
		 */
		int32_t read(uint8_t* rbuff, uint32_t rbsize)
		{
			if(_p_fp == nullptr || rbuff == nullptr) { return -1; }
			if(_p_rw.find("r") == std::string::npos && _p_rw.find("+") == std::string::npos) { return -2; }

			uint32_t _s_rbsize =  std::fread((void*)rbuff, sizeof(uint8_t), rbsize, _p_fp);
			if(rbsize > _s_rbsize)
			{
				if(std::feof(_p_fp) == 0u) { return -3; }
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

#endif /**/

// End of File
