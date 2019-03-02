/**
 * Some Basic Algorithm(merge sort, etc) for BWT2
 * 2019-03-01
 */

#ifndef __PRIVATE_BWT_H
#define __PRIVATE_BWT_H

#include <iostream>

namespace private_bwt
{
	/* Logic Suffix Array Definition */
	class suffixArray
	{
	public:
		uint8_t *nnstr;		/* !< the cache pointer */
		uint32_t position;  /* !< the position in origin Array */
		uint32_t length;	/* !< the length of nnstr */
		bool isCompleteSeq; /* !< the position is the end or not */

		/**
		 * @brief  Operator Reload(Equlation)
		 */
		void operator=(const suffixArray &b)
		{
			nnstr = b.nnstr;
			position = b.position;
			length = b.length;
			isCompleteSeq = b.isCompleteSeq;
		}

		/**
		 * @brief 	Operator Reload(Compare >)
		 */
		bool operator>(const suffixArray& toCompare)
		{
			uint32_t tmplen = length > toCompare.length ? toCompare.length : length;
			const uint8_t* seq1 = nnstr;
			const uint8_t* seq2 = toCompare.nnstr;

			while(tmplen --> 0u)
			{
				if(*seq1++ != *seq2++)
				{
					return (seq1[-1] < seq2[-1]) ? false : true;
				}
			}
			return true;
		}
	};

	/**
	 * @brief Private Algorithm of BWT: Merge Sort
	 */
	template <typename T>
	void __pbwt_merge_sort(T *arr, uint32_t left, uint32_t right);

}

/**
 * @brief 	Merge Elements after primer sort
 */
template <typename T>
void __pbwt_merge(T *a, uint32_t left, uint32_t right, uint32_t middle)
{
	uint32_t i = left, j = middle + 1u;

	T* aux = new T[right-left+1u];

	for (uint32_t k = left; k <= right; k++)
	{
		aux[k - left] = a[k];
	}

	for (uint32_t k = left; k <= right; k++)
	{
		if (i > middle)
		{
			a[k] = aux[j - left];
			j++;
		}
		else if (j > right)
		{
			a[k] = aux[i - left];
			i++;
		}
		else if (aux[i - left] > aux[j - left])
		{
			a[k] = aux[j - left];
			j++;
		}
		else
		{
			a[k] = aux[i - left];
			i++;
		}
	}

	delete[] aux;
}

/**
 * @brief 	Sub-progress of Merge Sort
 */
template <typename T>
void __pwbt_mergesort_primer(T *arr, uint32_t left, uint32_t right)
{
	if(left >= right)
	{
		return;
	}

	uint32_t middle = (left + right) / 2;

	__pwbt_mergesort_primer(arr, left, middle);
	__pwbt_mergesort_primer(arr, middle + 1, right);
	__pbwt_merge(arr, left, right, middle);
}

/**
 * @brief 	Definition of Merge Sort(called by bwt2.cpp)
 */
template<typename T>
void private_bwt::__pbwt_merge_sort(T *arr, uint32_t left, uint32_t right)
{
	__pwbt_mergesort_primer(arr, left, right-1u);
}

#endif /*__PRIVATE_BWT_H*/

/* End of File */
