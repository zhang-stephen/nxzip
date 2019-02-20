/**
 * A unistd.h implementation in Microsoft Windows Platform
 * 2019-02-20
**/

#ifndef __INSTEAD_UNISTD_H
#define __INSTEAD_UNISTD_H

#if linux || __linux__
	#include <unistd.h>
#elif WINNT || WIN32 || WIN64
	#include <io.h>
	#include <process.h>
#endif /* PLATFORM DEFINITIONS */

#endif /*__INSTEAD_UNISTD_H*/

/* End of File */
