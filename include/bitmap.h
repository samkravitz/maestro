/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: bitmap.h
 * DATE: March 9th, 2022
 * DESCRIPTION: bitmap manipulation utilities
 */

#ifndef BITMAP_H
#define BITMAP_H

#include <maestro.h>

void BITMAP_SET(u32 *bitmap, int bit)
{
	int i = bit / 32;
	int pos = bit % 32;
	u32 flag = 1 << pos;
	bitmap[i] |= flag;
}

void BITMAP_CLEAR(u32 *bitmap, int bit)
{
	int i = bit / 32;
	int pos = bit % 32;
	u32 flag = 1 << pos;
	bitmap[i] &= ~flag;
}

bool BITMAP_TEST(u32 *bitmap, int bit)
{
	int i = bit / 32;
	int pos = bit % 32;
	uint flag = 1 << pos;
	return (bitmap[i] & flag) != 0;
}

/**
 * @brief finds the first set bit of a bitmap
 * @param bitmap a pointer to the bitmap
 * @param max_bits the maximum bit index this bitmap keeps track of
 * @return index of first set bit or -1 if all are clear
 */
int BITMAP_FIRST_SET(u32 *bitmap, int max_bits)
{
	for (int i = 0; i < max_bits / 32; ++i)
	{
		// quickly check if all 32 bits of this u32 are clear to save some work
		if (bitmap[i] == 0)
			continue;

		// at least one of the bits in this u32 are set so find it
		for (int j = 0; j < 32; ++j)
		{
			int bit = i * 32 + j;
			if (BITMAP_TEST(bitmap, bit))
				return bit;
		}
	}

	return -1;
}

/**
 * @brief finds the first clear bit of a bitmap
 * @param bitmap a pointer to the bitmap
 * @param max_bits the maximum bit index this bitmap keeps track of
 * @return index of first clear bit or -1 if all are set
 */
int BITMAP_FIRST_CLEAR(u32 *bitmap, int max_bits)
{
	for (int i = 0; i < max_bits / 32; ++i)
	{
		// quickly check if all 32 bits of this u32 are set to save us some work
		if (bitmap[i] == 0xffffffff)
			continue;

		// at least one of the bits in this u32 are clear so find it
		for (int j = 0; j < 32; ++j)
		{
			int bit = i * 32 + j;
			if (!BITMAP_TEST(bitmap, bit))
				return bit;
		}
	}

	return -1;
}

#endif    // BITMAP_H
