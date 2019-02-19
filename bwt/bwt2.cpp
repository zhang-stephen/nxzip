/**
 * Another BWT Algorithm without the End Flag(to Compare with bwt.cpp)(source file)
 * 2019-02-19
**/ 

#include <iostream>
#include <vector>
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
	inline void nxz_mem_exchange(T* mem1, T* mem2, size_t len = sizeof(T))
	{
		T* tmpmem = new T;

		memcpy((void*)tmpmem, (void*)mem1, len);
		memcpy((void*)mem1, (void*)mem2, len);
		memcpy((void*)mem2, (void*)tmpmem, len);

		delete tmpmem;
	}
}

/**
 * @brief	The Burrows-Wheeler Transform 2 
 * @param 	uint8_t* srcArray: the Array to be transformed
 * @param	BWT* bwt: the result after BWT
 * @return	bool: true means no error occured
 * @note	!!! Length of srcArray must be as same as bwt->length
**/
bool NXZIP::NXZ_BWTransform2(uint8_t* srcArray, NXZIP::BWT* bwt)
{
	/* param check */
	if(nullptr == srcArray || nullptr == bwt || nullptr == bwt->cstr || 0u == bwt->length)
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
				nxz_mem_exchange(suf+j, suf+j+1);
			}

			if(::sequence_cmp2(suf[j].nnstr, suf[j+1].nnstr, tmplen) == 1)
			{
				nxz_mem_exchange(suf+j, suf+j+1);
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

/* End of File */
