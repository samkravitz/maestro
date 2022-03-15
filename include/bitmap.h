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

#define BITMAP_SET(bitmap, bit)   (bitmap[bit / 32] |= (1 << (bit % 32)))
#define BITMAP_CLEAR(bitmap, bit) (bitmap[bit / 32] &= ~(1 << (bit % 32)))
#define BITMAP_TEST(bitmap, bit)  (bitmap[bit / 32] & (1 << (bit % 32)) & 1)

/**
 * @brief finds the first set bit of a bitmap
 * @param bitmap a pointer to the bitmap
 * @param max_bits the maximum bit index this bitmap keeps track of
 * @return index of first set bit or -1 if all are clear
 */
inline int BITMAP_FIRST_SET(u32 *bitmap, int max_bits)
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
inline int BITMAP_FIRST_CLEAR(u32 *bitmap, int max_bits)
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
