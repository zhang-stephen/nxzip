/**
 * File Operation Functions(Source file)
 * 2019-04-10
 */

#include <cstdio>
#include <algorithm>
#include <filesystem>

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
		_io_file() { _p_fp = nullptr; }

		~_io_file()
		{
			if(_p_fp != nullptr)
			{
				std::fclose(_p_fp);
			}
		}

		uint8_t open(const char* fn, char* rw, bool fflag = false)
		{
			if(_p_fp != nullptr) { std::fclose(_p_fp); }
			_p_fp = std::fopen(fn, rw);
			_p_rw = rw;
			_p_fflag = fflag;
			return (_p_fp == nullptr ? 1u : 0u);
		}

		int8_t write(uint8_t* wbuff, uint32_t wbsize)
		{
			if(_p_fp == nullptr) { return -1; }
			if(_p_rw != "w" && _p_rw != "w+" && _p_rw != "r+" && _p_rw != "a+") { return -2; }

			if(wbsize != std::fwrite((const void*)wbuff, sizeof(uint8_t), wbsize, _p_fp)) { return -3; }
			else 
			{
				if(_p_fflag)
				{ 
					if(std::fflush(_p_fp) != 0) { return 1u; }
				}
				return 0u; 
			}
		}

		int8_t read(nxzutil::VLBUFF& rbuff, uint32_t rbsize)
		{
			if(_p_fp == nullptr) { return -1; }
			if(_p_rw != "r" && _p_rw != "w+" && _p_rw != "r+" && _p_rw != "a+") { return -2; }

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
	};
}

/* End of File */
