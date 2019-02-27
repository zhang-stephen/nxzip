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
	/* Logic Suffix Array Definition */
	typedef struct 
	{
		uint8_t* nnstr;			/* !< the cache pointer */
		uint32_t position;		/* !< the position in origin Array */
		uint32_t length;		/* !< the length of nnstr */
		bool isCompleteSeq;		/* !< the position is the end or not */
	}SuffixArray2_TypeDef;

	/**
	 * @brief	Compare sequences by subscript(be like memcmp)
	 */
	template<typename T> 
	int sequence_cmp2(T* s1, T* s2, size_t count = sizeof(T))
	{
		const T* seq1 = s1;
		const T* seq2 = s2;

		while(count --> 0u)
		{
			if(*seq1++ != *seq2++)
			{
				return (seq1[-1] < seq2[-1]) ? -1 : 1;
			}
		}
		return 0;
	}

	/**
	 * @brief	Exchange specified memory ereas
	 */
	template<typename T>
	inline void memoryExchange(T* mem1, T* mem2, size_t len = sizeof(T))
	{
		T* tmpmem = new T;

		memcpy((void*)tmpmem, (void*)mem1, len);
		memcpy((void*)mem1, (void*)mem2, len);
		memcpy((void*)mem2, (void*)tmpmem, len);

		delete tmpmem;
	}

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

		while(len --> 0u)
		{
			if(seq[i] == elem)
				tmptimes++;

			if(tmptimes == times)
				break;

			i++;
		}

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

	/* Allocate the Memory */
	SuffixArray2_TypeDef* suf = new SuffixArray2_TypeDef[bwt->length];
	SuffixArray2_TypeDef* TMP = new SuffixArray2_TypeDef{nullptr, 0u, 0u, false};
	uint32_t tmplen = 0;

	/* Init the logic suffix array */
	for(uint32_t i = 0; i < bwt->length; i++)
	{
		suf[i].nnstr = srcArray + i;
		suf[i].position = i;
		suf[i].length = bwt->length - i;
		suf[i].isCompleteSeq = (suf[i].position == 0);
	}

	/* Bubble sort for Logic Suffix Array */
	/* Time Complicity: O(n^3) */
	/* TODO: Bubble sort --> Quick sort */
	/* Time Complexity: O(n^3) */
	for(uint32_t i = 0; i < bwt->length - 1u; i++)
	{
		for(uint32_t j = 0; j < bwt->length - 1u; j++)
		{
			tmplen = ((suf[j].length < suf[j+1].length) ? suf[j].length : suf[j+1].length);

			if(::sequence_cmp2(suf[j].nnstr, suf[j+1].nnstr, tmplen) == 0 && suf[j].length > suf[j+1].length)
			{
				memoryExchange(suf+j, suf+j+1);
			}

			if(::sequence_cmp2(suf[j].nnstr, suf[j+1].nnstr, tmplen) == 1)
			{
				memoryExchange(suf+j, suf+j+1);
			}
		}
	}

	/* take out the result atfter BWT */
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
bool NXZIP::NXZ_BWTransform_Inverse2(NXZIP::BWT* ibwt, uint32_t length, uint8_t* dstArray)
{
	/* param check */
	if(nullptr == dstArray || nullptr == ibwt || nullptr == ibwt->cstr || 0u == ibwt->length)
	{
		return false;
	}

	/* Allocate the memory */
	uint32_t cursor = ibwt->index;				/* !< the position to take out element in Last Array  */
	uint32_t tmplen = length, tmp = 0u;
	uint8_t* firstColumn = new uint8_t[length];
	uint8_t* lastColumn = ibwt->cstr;

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
