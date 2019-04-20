/**
 * Another BWT Algorithm without the End Flag(to Compare with bwt.cpp)(source file)
 * 2019-02-19
**/ 

#include <iostream>
#include <algorithm>
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
	
	/**
	 * @brief get the index of specific element
	 */
	template<typename T>
	inline int64_t getIndexViaTimes(T* seq, T elem, size_t len, uint32_t times)
	{
		/* parameters check */
		if(nullptr == seq || times == 0 || times > len)
		{
			return -1;
		}

		int64_t i = 0u;
		uint32_t tmptimes = 0u;

		/* Search Element via appearance times */
		while(len --> 0u)
		{
			if(seq[i] == elem)
				tmptimes++;

			if(tmptimes == times)
				break;

			i++;
		}

		/* found nothing */
		if(tmptimes == 0u || tmptimes < times)
			i = -1;

		return i;
	}

	/**
	 * @brief Get the Times in specific memory area
	 */
	template<typename T>
	inline int64_t getTimesViaIndex(T* seq, T elem, size_t posBegin, size_t posEnd)
	{
		/* Parametes Check */
		if(nullptr == seq || posBegin > posEnd)
		{
			return -1;
		}

		uint32_t times = 0u;

		for(size_t i = posBegin; i < posEnd; i++)
		{
			if(seq[i] == elem)
			{
				times++;
			}
		}

		return times;
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
int cmp(struct suffix a, struct suffix b) 
{ 
	return (a.rank[0] == b.rank[0])? (a.rank[1] < b.rank[1] ?1: 0): 
			(a.rank[0] < b.rank[0] ?1: 0); 
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
	::suffix* suffixes = new ::suffix[length];

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
	std::sort(suffixes, suffixes+length, cmp); 

	// At this point, all suffixes are sorted according to first 
	// 2 characters. Let us sort suffixes according to first 4 
	// characters, then first 8 and so on 
	int32_t ind[length]; // This array is needed to get the index in suffixes[] 
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
		std::sort(suffixes, suffixes+length, cmp); 
	}

	// take out the result
	for(int32_t i = 0u; i < length; i++)
	{
		tmp = suffixes[i].index;
		bwt->cstr[i] = (tmp != 0u) ? srcArray[tmp-1u] : srcArray[length-1u];

		if(tmp == 0u) { bwt->index = i - 1u; }
	}

	delete[] suffixes;

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

	/* Allocate the memory */
	uint32_t cursor = index;
	uint32_t tmplen = length, tmp = 0u;
	uint8_t* firstColumn = new uint8_t[length];
	uint8_t* lastColumn = srcArray;

	/* Construct the first Column */
	std::memcpy(firstColumn, lastColumn, length);
	std::sort(firstColumn, firstColumn+length);

	while(tmplen --> 0u)
	{
		dstArray[tmplen] = lastColumn[cursor];

		tmp = (uint32_t)(::getTimesViaIndex(lastColumn, lastColumn[cursor], 0, cursor+1u));
		cursor = (uint32_t)(::getIndexViaTimes(firstColumn, lastColumn[cursor], length, tmp));
	}

	/* Destroy Allocated Memory */
	delete[] firstColumn;

	return true;
}

/* End of File */
