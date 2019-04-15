/**
 * Run-Length Code Algorithm(source file)
 * 2019-03-30
 */

#include <iostream>
#include <vector>

#include "rlc.h"

namespace
{
	bool isRepeatGreaterThan3Times(uint8_t* buff, uint32_t remainLen);
	uint8_t getNoRepeatBytesCount(uint8_t* buff, uint32_t remainLen);
	uint8_t getRepeatBytesCount(uint8_t* buff, uint32_t remainLen);
}

bool isRepeatGreaterThan3Times(uint8_t* buff, uint32_t remainLen)
{
	if(remainLen < 2u) { return false; }
	else
	{
		if(*buff == *(buff+1u) && *buff == *(buff+2u)) { return true; }
		else { return false; }
	}
}

uint8_t getNoRepeatBytesCount(uint8_t* buff, uint32_t remainLen)
{
	uint32_t c = 0u, remainc = remainLen, tmp = 0u;
	uint8_t* tmpptr = buff;

	if(remainLen < 2u) { return remainLen + 1u; }
	else
	{
		while(::isRepeatGreaterThan3Times(tmpptr, remainc) == false)
		{
			tmpptr++; remainc--; c++;

			if(c >= 128u)
			{
				tmp = c; break;
			}

			if(remainc == 0u)
			{
				tmp = c + 1u; break;
			}
		}

		tmp = c;
	}

	return tmp;
}

uint8_t getRepeatBytesCount(uint8_t* buff, uint32_t remainLen)
{
	uint32_t c = 0u, remainc = remainLen, tmp = 0u;
	uint8_t* tmpptr = buff;

	if(remainLen < 2u) { return remainLen + 1u; }
	else
	{
		// while(::isRepeatGreaterThan3Times(tmpptr, remainc))
		while(*(tmpptr) == *(tmpptr+1u))
		{
			tmpptr++; remainc--; c++;

			if(c >= 128u)
			{
				tmp = c; break;
			}

			if(remainc == 0u)
			{
				tmp = c + 1u; break;
			}
		}

		tmp = c + 1u;
	}

	return tmp;
}

/**
 * @brief	Run-Length Encoding
 * @param	uint8_t* srcBuff
 * 				prime sequence to encode
 * @param	uint32_t srcLength
 * 				length of prime sequence
 * @param	utility::VLBUFF* oBuff
 * 				output buffer
 * @return	bool
 * @note	None
 */
bool NXZIP::NXZ_RunLengthEncode(uint8_t* srcBuff, uint32_t srcLength, utility::VLBUFF* oBuff)
{
	/* Parameters Check */
	if(srcBuff == nullptr || srcLength == 0u)
	{
		return false;
	}

	uint8_t* tmpsrc = srcBuff;
	uint32_t remain = srcLength;
	uint8_t count = 0u;
	std::vector<uint8_t> tmpRLC(0u, 0u);

	/* Encoding */
	while(remain > 0u)
	{
		/* repeat data occured */
		if(::isRepeatGreaterThan3Times(tmpsrc, remain))
		{
			count = ::getRepeatBytesCount(tmpsrc, remain);
			tmpRLC.push_back(count | 0x80u);
			tmpRLC.push_back(*tmpsrc);
			tmpsrc += count;
			remain -= count;
		}
		/* no repeat data */
		else
		{
			count = ::getNoRepeatBytesCount(tmpsrc, remain);
			tmpRLC.push_back(count);

			for(uint8_t i = 0u; i < count; i++)
			{
				tmpRLC.push_back(*tmpsrc++);
			}
			remain -= count;
		}
	}

	/* copy data */
	oBuff->allocate(tmpRLC.size());
	std::copy(tmpRLC.begin(), tmpRLC.end(), oBuff->uptr);

	return true;
}

/**
 * @brief	Run-Length Decoding
 */
bool NXZIP::NXZ_RunLengthDecode(uint8_t* rlcBuff, uint32_t rlcLength, utility::VLBUFF* yBuff)
{
	/* Parameters Check */
	if(rlcBuff == nullptr || rlcLength == 0u)
	{
		return false;
	}

	uint8_t* tmpbuff = rlcBuff;
	uint32_t remain = 0u;
	uint8_t count = rlcLength, sign = 0u; //sign is repeat count with flag bit
	std::vector<uint8_t> tmpDst(0u, 0u);

	/* Decode */
	while(tmpbuff < (rlcBuff + rlcLength))
	{
		sign = *tmpbuff++;
		count = sign & 0x7Fu;

		/* repeat data */
		if((sign & 0x80u) == 0x80u)
		{
			for(uint8_t i = 0u; i < count; i++)
			{
				tmpDst.push_back(*tmpbuff);
			}
			tmpbuff++;
		}
		/* no repeat data */
		else
		{
			for(uint8_t i = 0; i < count; i++)
			{
				tmpDst.push_back(*tmpbuff++);
			}
		}
		
	}

	/* copy data */
	yBuff->allocate(tmpDst.size());
	std::copy(tmpDst.begin(), tmpDst.end(), yBuff->uptr);

	return true;
}

/* End of File */
