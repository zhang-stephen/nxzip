/**
 * Another BWT Algorithm without the End Flag(to Compare with bwt.cpp)(source file)
 * 2019-02-19
**/ 

#include <iostream>
#include <algorithm>
#include <vector>
#include <cstring>

#include "bwt.h"

/* Anoymous Namespace */
namespace
{
	/* Auxiliary Structure to construct Suffix Array */
	struct suffix
	{
		uint32_t index;
		int32_t rank[2];

		/* default constructor */
		suffix() { index = 0u; rank[0] = 0u; rank[1] = 0u; }
	};
	
	/* bucket Sort */
	void char_sort_to(const char *base, size_t sz, char *output) 
	{
		uint32_t buckets[256];
		memset(buckets, 0, sizeof(uint32_t)*256);
		size_t i = 0;

		for (; i + 4 < sz; i += 4) 
		{
			const char *ptr = base + i;
			buckets[(uint8_t)ptr[0]]++;
			buckets[(uint8_t)ptr[1]]++;
			buckets[(uint8_t)ptr[2]]++;
			buckets[(uint8_t)ptr[3]]++;
		}

		for (; i < sz; i++) 
		{
			const char *ptr = base + i;
			buckets[(uint8_t)ptr[0]]++;
		}

		for (uint16_t i = 0; i < 256; i++) {
			memset(output, static_cast<char>(i), buckets[i]);
			output += buckets[i];
		}
	}
}

/* Public Class BWT functions */

/**
 * @brief	The Constructor of class BWT
**/
NXZIP::BWT::BWT(uint32_t size)
{
	length = size; 
	cstr = new uint8_t[size];
	index = 0u;
}

/**
 * @brief	the Destructor of class BWT 
**/
NXZIP::BWT::~BWT(void)
{
	length = 0u;
	index = 0u;
	delete[] cstr;
}

/**
 * @brief	Compare Policy for std::sort
 */
bool cmp(struct suffix& a, struct suffix& b) 
{ 
	return (a.rank[0] == b.rank[0]) ? (a.rank[1] < b.rank[1] ? true : false): 
			(a.rank[0] < b.rank[0] ? true : false); 
} 

/**
 * @brief	The Burrows-Wheeler Transform 2 
 * @param 	uint8_t* srcArray: the Array to be transformed
 * @param	uint32_t length: length of srcArray(only for verifying)
 * @param	BWT* bwt: the result after BWT
 * @return	bool: true means no error occured
 * @note	!!! Length of srcArray must be as same as bwt->length
**/
bool NXZIP::NXZ_BWTransform2(uint8_t* srcArray, uint32_t length, NXZIP::BWT* bwt)
{
	/* param check */
	if(nullptr == srcArray || nullptr == bwt || nullptr == bwt->cstr || 0u == bwt->length || length != bwt->length)
	{
		return false;
	}

	/* source array length must in [1ul, 0x7ffffffful] */
	if(length > 0x7FFFFFFFul || length == 0u)
	{
		return false;
	}

	/* create temporary variables */
	// A structure to store suffixes and their indexes 
	std::vector<::suffix> suffixes(length);

	// A Temporary Variable
	uint32_t tmp = 0u;

	// Store suffixes and their indexes in an array of structures. 
	// The structure is needed to sort the suffixes alphabatically 
	// and maintain their old indexes while sorting 
	for(int i = 0; i < length; i++) 
	{ 
		suffixes[i].index = i; 
		suffixes[i].rank[0] = srcArray[i];
		suffixes[i].rank[1] = ((i+1) < length) ? srcArray[i + 1u] : -1; 
	}

	// Sort the suffixes using the comparison function 
	// defined above. 
	std::sort(suffixes.begin(), suffixes.end(), cmp);

	// At this point, all suffixes are sorted according to first 
	// 2 characters. Let us sort suffixes according to first 4 
	// characters, then first 8 and so on 
	std::vector<int32_t> ind(length); // This array is needed to get the index in suffixes[] 
				// from original index. This mapping is needed to get 
				// next suffix. 
	for (int32_t k = 4; k < 2u * length; k *= 2u) 
	{ 
		// Assigning rank and index values to first suffix 
		int32_t rank = 0; 
		int32_t prev_rank = suffixes[0].rank[0]; 
		suffixes[0].rank[0] = rank; 
		ind[suffixes[0].index] = 0; 

		// Assigning rank to suffixes 
		for (int32_t i = 1; i < length; i++) 
		{ 
			// If first rank and next ranks are same as that of previous 
			// suffix in array, assign the same new rank to this suffix 
			if (suffixes[i].rank[0] == prev_rank && 
					suffixes[i].rank[1] == suffixes[i-1].rank[1]) 
			{ 
				prev_rank = suffixes[i].rank[0]; 
				suffixes[i].rank[0] = rank; 
			} 
			else // Otherwise increment rank and assign 
			{ 
				prev_rank = suffixes[i].rank[0]; 
				suffixes[i].rank[0] = ++rank; 
			} 
			ind[suffixes[i].index] = i; 
		} 

		// Assign next rank to every suffix 
		for (int32_t i = 0; i < length; i++) 
		{ 
			int32_t nextindex = suffixes[i].index + k/2; 
			suffixes[i].rank[1] = (nextindex < length)? 
								suffixes[ind[nextindex]].rank[0] : -1; 
		} 

		// Sort the suffixes according to first k characters 
		std::sort(suffixes.begin(), suffixes.end(), cmp);
	}

	// take out the result
	for(int32_t i = 0u; i < length; i++)
	{
		tmp = suffixes[i].index;
		bwt->cstr[i] = (tmp != 0u) ? srcArray[tmp-1u] : srcArray[length-1u];

		if(tmp == 0u) { bwt->index = i - 1u; }
	}

	return true;
}

/**
 * @brief 	The Inverse-BWT Algorithm 2
 * @param 	BWT* ibwt: the BWT Data Structure(member length maybe zero)
 * @param 	uint32_t length: the length of dstArray
 * @param 	uint8_t* dstArray: the result of IBWT 
 * @return 	bool: true means no error occured
 * @note	!!! Length of srcArray must be as same as bwt->length
 */
bool NXZIP::NXZ_BWTransform_Inverse2(uint8_t* srcArray, uint32_t length, uint32_t index, uint8_t* dstArray)
{
	/* param check */
	if(nullptr == dstArray || nullptr == srcArray || 0u == length)
	{
		return false;
	}

	char *F = new char[length];
    uint32_t *C = new uint32_t[length];
    char_sort_to((const char*)srcArray, length, F);

    /* marks first occurrence of char in F */
    int32_t M[256];
    memset(M, 0xff, sizeof(int32_t)*256);
    for (size_t i = 0; i < length; i++) 
	{
    	uint8_t k = F[i];
    	if (M[k] < 0) { M[k] = i; }
    }

    /* marks number of occurences of L[j] in L for all j */
    uint32_t tmp[256];
    memset(tmp, 0, sizeof(int32_t)*256);
    memset(C, 0, sizeof(uint32_t)*length);
    for (size_t i = 0; i < length; i++) 
	{
        uint8_t k = srcArray[i];
        C[i] = tmp[k]++;
    }

    /* compute ibwt backwards */
    char *buf = new char[length];
    char *ptr = buf + length - 1;
    *ptr-- = F[0];
    uint32_t j = 0;
    for (size_t i = 1; i < length; i++) 
	{
        *ptr-- = srcArray[j];
        j = M[(uint8_t)srcArray[j]] + C[j];
    }

	std::copy(buf, buf+length, dstArray);

    delete[] F;
    delete[] C;
    delete[] buf;

	return true;
}

/* End of File */
