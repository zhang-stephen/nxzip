/**
 * New Run-Length Encoding for output sequence of MTF(Source file)
 * 2019-04-12
 */

#include "rlc.h"
#include <algorithm>
#include <vector>

int8_t isZeroRepeatMoreThan2Times(uint8_t* buff, uint32_t length);
uint16_t getCountOfZeroRepeats(uint8_t* ptr, uint32_t length);
uint8_t getCountOfNonZeros(uint8_t* ptr, uint32_t length);
uint16_t _m_f_power(uint8_t a, uint8_t b);
int16_t _m_log_b2(uint8_t n);

int8_t isZeroRepeatMoreThan2Times(uint8_t* buff, uint32_t length)
{
	/* parameters check */
	if(buff == nullptr || length == 0u) { return -1; }
	if(length <= 2u) { return 1u; }

	/* check elements */
	if(*buff == 0u && *buff == *(buff+1u) && *buff == *(buff+2u)) { return 0u; }
	else { return 1u; }
}

uint16_t getCountOfZeroRepeats(uint8_t* ptr, uint32_t length)
{
	uint16_t c = 0u;
	uint8_t* tmpptr = ptr;

	if(length < 2u) { return length + 1u; }
	else
	{
		while(*tmpptr == 0u && *tmpptr == *(tmpptr+1u))
		{
			tmpptr++, c++; length--;

			if(length == 0u) { return c + 1u; }
			if(c == 0x0FFFu) { return c; }			// max count: 4095ul
		}

		return c + 1u;
	}
}

uint8_t getCountOfNonZeros(uint8_t* ptr, uint32_t length)
{
	uint8_t c = 0u;
	uint8_t* tmpptr = ptr;

	if(length <= 2u) { return length; }
	else
	{
		while(*tmpptr != 0u || !(*tmpptr == 0u && *(tmpptr+1u) == 0u && *(tmpptr+2u) == 0u))
		{
			tmpptr++; c++; length--;
			if(length == 0u) { return c + 1u; }
			if(c == 0x7Fu) { return c; }
		}

		return c;
	}
}

/**
 * @brief Calculate the result of a^b
 */
uint16_t _m_f_power(uint8_t a, uint8_t b)
{
	if(a == 0u) { return 0u; }
	if(b == 0u && a != 0u) { return 1u; }

	int16_t res = 1u, base = a;
	while(b != 0u)
	{
		if(b % 2u != 0u) { res *= base; }
		base *= base;
		b /= 2u;
	}

	return res;
}

/**
 * @brief	Fast method to calculate [log(2, n+1)](get integer)
 */
int16_t _m_log_b2(uint16_t n)
{
	uint8_t tmpl = 0u;

	if(n == 0u) 
	{ 
		return -1;
	}
	else
	{
		while(true)
		{
			if((n+1u) / _m_f_power(2u, tmpl) != 0u)
			{
				if((n+1u) / _m_f_power(2u, tmpl+1u) == 0u) { break; }
				else { tmpl++; }
			}
		}
	}

	return tmpl;
}

/**
 * @brief	Run-Length Encoding for MTF Sequence
 * @param	uint8_t* src
 * 				source sequence to encode
 * @param	uint32_t srcLength
 * 				length of source sequence
 * @param	utility::vlbuff* rlcbuff
 * 				rlc buffer(return, in namespace NXZIP)
 * @return	true if no error occured
 * @note	None
 */
bool NXZIP::NXZ_mRunLength_Encoding(uint8_t* src, uint32_t srcLength, utility::VLBUFF* rlcbuff)
{
	/* parameters check */
	if(src == nullptr || srcLength == 0u || rlcbuff == nullptr)
	{
		return false;
	}

	/* create Temporary Data */
	std::vector<uint8_t> tmpvec;
	uint8_t* tmpsrc = src;
	uint8_t es_c1 = 0u;		// es_cx: Escape Character x(x = 1, 2)
	uint16_t es_c2 = 0u, count = 0u;				// temporary of es_c2
	uint32_t tmplen = srcLength;

	/* Encoding */
	while(tmpsrc - src < tmplen)
	{
		/* replace contionus zero sequence by two escape characters */
		if(isZeroRepeatMoreThan2Times(tmpsrc, srcLength) == 0u)
		{
			count = getCountOfZeroRepeats(tmpsrc, srcLength);
			es_c1 = 0x80u | (_m_log_b2(count) << 3u);
			es_c2 = count + 1u - _m_f_power(2u, _m_log_b2(count));
			tmpvec.push_back(es_c1 | ((es_c2 & 0x0700u) >> 8u));
			tmpvec.push_back((uint8_t)es_c2);
			es_c1 = 0u; es_c2 = 0u;
			tmpsrc += count; srcLength -= count;
		}
		/* replace other sequence by one escape character */
		else if(isZeroRepeatMoreThan2Times(tmpsrc, srcLength) == 1u)
		{
			count = getCountOfNonZeros(tmpsrc, srcLength) & 0x7Fu;
			tmpvec.push_back(count);

			for(uint8_t i = 0; i < count; i++)
			{
				tmpvec.push_back(*tmpsrc++);
			}

			srcLength -= count;
		}
		else
		{
			return false;
		}
	}

	/* take out data */
	rlcbuff->allocate(tmpvec.size());
	std::copy(tmpvec.begin(), tmpvec.end(), rlcbuff->uptr);

	return true;
}

bool NXZIP::NXZ_mRunLength_Decoding(uint8_t* rlc, uint32_t rlcLength, utility::VLBUFF* oribuff)
{
	/* parameters check */
	if(rlc == nullptr || rlcLength == 0u || oribuff == nullptr)
	{
		return false;
	}

	/* create temporary variables */
	std::vector<uint8_t> tmpvec;
	uint16_t mcount = 0u, tmp = 0u;
	uint8_t* tmprlc = rlc;
	bool _r_flag = false;

	/* decoding */
	while(tmprlc - rlc < rlcLength)
	{
		/* read 1st escape character */
		if((*tmprlc & 0x80u) != 0u)	// zero sequence
		{
			/* read 2nd escape character */
			tmp = ((*tmprlc & 0x03u) << 8u) | *(tmprlc+1u);
			mcount = _m_f_power(2u, (*tmprlc & 0x78u) >> 3u) + tmp - 1u;
			tmprlc += 2u;

			for(uint16_t i = 0u; i < mcount; i++)
			{
				tmpvec.push_back(0u);
			}
		}
		else 						// non-zero sequnce
		{
			mcount = *tmprlc & 0x7Fu; tmprlc++;

			for(uint16_t i = 0; i < mcount; i++)
			{
				tmpvec.push_back(*tmprlc++);
			}
		}
	}

	/* take out the result */
	oribuff->allocate(tmpvec.size());
	std::copy(tmpvec.begin(), tmpvec.end(), oribuff->uptr);

	return true;
}

/* End of file */
