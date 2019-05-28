/**
 * A Test Function of NXZIP with CLI-options(header file)
 * 2019-05-28
 */

#ifndef __NTEST_H
#define __NTEST_H

#include <string>

namespace NXZIP
{
	void NXZ_BasicStream_Test(const std::string& ifile);
	void NXZ_CombineStream_Test(const std::string& ifile);
}

#endif /*__NTEST_H*/

/* End of File */