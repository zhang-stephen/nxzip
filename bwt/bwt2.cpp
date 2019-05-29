/**
 * Another BWT Algorithm without the End Flag(to Compare with bwt.cpp)(source file)
 * 2019-02-19
**/ 

#include <iostream>
#include <algorithm>
#include <vector>
#include <cstring>
#include <numeric>

#include "bwt.h"
#include "../nxzutil.h"

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

		if(tmp == 0u) { bwt->index = i; }
	}

	return true;
}

/**
 * @brief 	The Inverse-BWT Algorithm 2
 */
bool NXZIP::NXZ_BWTransform_Inverse2(uint8_t* srcArray, uint32_t length, uint32_t index, uint8_t* dstArray)
{
	/* param check */
	if(nullptr == dstArray || nullptr == srcArray || 0u == length)
	{
		return false;
	}

	uint8_t* L = srcArray;

	/* code based on pseudo code from section 4.2 (D1 and D2) follows */
	uint32_t count[256] = {0u};
	std::vector<uint32_t> pred(length, 0);

	/*******************************************************************
	 * Set pred[i] to the number of times block[i] appears in the
	 * substring block[0 .. i - 1].  As a useful side effect count[i]
	 * will be the number of times character i appears in block.
	*******************************************************************/
	for (uint32_t i = 0; i < length; i++)
	{
		pred[i] = count[L[i]];
		count[L[i]]++;
	}

	/*******************************************************************
	 * Finally, set count[i] to the number of characters in block
	 * lexicographically less than i.
	*******************************************************************/
	uint32_t sum = 0;
	for(uint32_t i = 0; i <= 255; i++)
	{
		uint32_t j = count[i];
		count[i] = sum;
		sum += j;
	}

	/* construct the initial unrotated string (S[0]) */
	uint32_t i = index;
	for(uint32_t j = length; j > 0; j--)
	{
		dstArray[j - 1] = L[i];
		i = pred[i] + count[L[i]];
	}

	return true;
}

/* End of File */
