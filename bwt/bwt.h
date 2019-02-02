/**
 * BWT Algorithm (header file)
 * 2019-02-02
 */

#ifndef __BWT_H
#define __BWT_H

#include <cstdint>

namespace NXZIP
{
	class BWT_CoreClass
	{
	public:

	private:
		uint32_t index;
		uint8_t* cstr;
	};
	
	bool NXZ_BWTransform();
}

#endif /*__BWT_H*/

//EOF
