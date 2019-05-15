/**
 * logger module header file
 * 2019-04-29
 */

#ifndef __T_LOGGER_H
#define __T_LOGGER_H

#include <cstdio>

#include "unistd.h"

#ifdef NXZ_DEBUG
	#define NXZ_TRACE_INFO(fmt,...) \
		std::printf("%s[%d]-[%s]: "#fmt"\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
	
	#define NXZ_PRINT(fmt,...) \
		std::printf("%s[%d]-[%s]: "#fmt"\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#else
	#define NXZ_TRACE_INFO(fmt,...) ((void)0u)

	#define NXZ_PRINT(fmt,...) \
		std::printf(""#fmt"\n", ##__VA_ARGS__)
#endif /*NXZIP_INFO_TRACK*/

#endif /*__T_LOGGER_H*/

/* End of File */
