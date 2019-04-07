/**
 * Another BWT Algorithm without the End Flag(to Compare with bwt.cpp)(source file)
 * 2019-02-19
**/ 

#include <iostream>
#include <algorithm>
#include <cstring>
#include "bwt.h"
#include "pbwt.h"

/* Anoymous Namespace */
namespace
{
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
	this->length = size; 
	this->cstr = new uint8_t[size];
	this->index = 0u;
}

/**
 * @brief	the Destructor of class BWT 
**/
NXZIP::BWT::~BWT(void)
{
	this->length = 0u;
	this->index = 0u;
	delete[] cstr;
}

/**
 * @brief	get string pointer in c style
 */
uint8_t* NXZIP::BWT::c_str(void)
{
	return cstr;
}

/**
 * @brief	get the index of BWT Sequence
 */
uint32_t NXZIP::BWT::getIndex(void)
{
	return index;
}

/**
 * @brief	re-allocate memory if length changed
 */
void NXZIP::BWT::reallocateMem(uint32_t newLength)
{
	if(newLength == length) { return ; }

	delete[] cstr;
	length = newLength;
	cstr = new uint8_t[newLength];
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

	using namespace private_bwt;

	/* Allocate the Memory */
	suffixArray* suf = new suffixArray[bwt->length];
	suffixArray* TMP = new suffixArray{nullptr, 0u, 0u, false};
	uint32_t tmplen = 0;

	/* Init the logic suffix array */
	for(uint32_t i = 0; i < bwt->length; i++)
	{
		suf[i].nnstr = srcArray + i;
		suf[i].position = i;
		suf[i].length = bwt->length - i;
		suf[i].isCompleteSeq = (suf[i].position == 0);
	}

	/* Logic Suffix Array Sort: Merge Sort */
	/* Time Complexity: O(n^2logn) */
	/* Space Complexity: T(n) */
	/* TODO: optimization --> Tim Sort */
	__pbwt_merge_sort(suf, 0, bwt->length);

	/* take out the result after BWT */
	for(uint32_t i = 0u; i < bwt->length; i++)
	{
		bwt->cstr[i] = (suf[i].position == 0u) ? srcArray[bwt->length-1u] : suf[i].nnstr[-1];

		if(suf[i].isCompleteSeq == true)
		{
			bwt->index = i;
		}
	}

	/* Free the Memory */
	delete[] suf;
	delete TMP;

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
	memcpy(firstColumn, lastColumn, length);
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
