/* maestro
 * License: GPLv2
 * See LICENSE.txt for full license text
 * Author: Sam Kravitz
 *
 * FILE: libc/string.c
 * DATE: July 27, 2021
 * DESCRIPTION: memcmp, memcpy, memset, strcat, strncat, strlen
 */

#include "string.h"

#include <kmalloc.h>

// compares n bytes of s1 with s2
// returns:
//   <0 if s1 < s2
//   >0 if s1 > s2
//   =0 if s1 = s2
int memcmp(const void *s1, const void *s2, size_t n)
{
	const char *a = (const char *) s1;
	const char *b = (const char *) s2;

	while (n--)
	{
		if (*a < *b)
			return -1;
		else if (*a > *b)
			return 1;

		a++;
		b++;
	}

	return 0;
}

// copies n bytes of memory from src to dest
void *memcpy(void *dest, const void *src, size_t n)
{
	char *dp       = (char *) dest;
	const char *sp = (const char *) src;

	while (n--)
		*dp++ = *sp++;

	return dest;
}

// copy memory area
void *memmove(void *dest, const void *src, size_t n)
{
	char *d       = (char *) dest;
	const char *s = (const char *) src;

	// copy from end so the buffer's don't overwrite one another
	if ((uintptr_t) src < (uintptr_t) dest)
	{
		d += n - 1;
		s += n - 1;
		while (n--)
			*d-- = *s--;
	}

	// copy from beginning (basically just memcpy)
	else
	{
		while (n--)
			*d++ = *s++;
	}

	return dest;
}

// sets n bytes of ptr to c
void *memset(void *ptr, int c, size_t n)
{
	char *p = (char *) ptr;

	while (n--)
		*p++ = c;

	return ptr;
}

char *strcat(char *dest, const char *src)
{
	size_t i = 0, len = strlen(dest);

	while (src[i])
	{
		dest[i + len] = src[i];
		i++;
	}

	dest[i + len] = '\0';

	return dest;
}

char *strncat(char *dest, const char *src, size_t n)
{
	size_t i = 0, len = strlen(dest);

	while (src[i] && n--)
	{
		dest[i + len] = src[i];
		i++;
	}

	dest[i + len] = '\0';

	return dest;
}

char *strcpy(char *dest, const char *src)
{
	return strncpy(dest, src, strlen(src) + 1);
}

char *strncpy(char *dest, const char *src, size_t n)
{
	memcpy(dest, src, n);
	return dest;
}

// returns a pointer to the first occurrence of c in str,
// or NULL if it is not found.
// the null terminator is considered part of the string
char *strchr(const char *str, char c)
{
	for (size_t i = 0; i < strlen(str) + 1; i++)
	{
		if (str[i] == c)
			return (char *) &str[i];
	}

	return NULL;
}

// returns a pointer to the last occurrence of c in str,
// or NULL if it is not found.
// the null terminator is considered part of the string
char *strrchr(const char *str, char c)
{
	size_t i = strlen(str) + 1;
	while (i-- != 0)
	{
		if (str[i] == c)
			return (char *) &str[i];
	}

	return NULL;
}

int strcmp(const char *a, const char *b)
{
	return strncmp(a, b, strlen(a));
}

int strncmp(const char *a, const char *b, size_t n)
{
	const u8 *s1 = (const u8 *) a;
	const u8 *s2 = (const u8 *) b;

	while (n && *s1 && (*s1 == *s2))
	{
		s1++;
		s2++;
		n--;
	}

	return n == 0 ? 0 : *s1 - *s2;
}

char *strdup(const char *str)
{
	return strndup(str, strlen(str));
}

char *strndup(const char *str, size_t n)
{
	char *dup = (char *) kmalloc(n + 1);
	memcpy(dup, str, n);
	dup[n] = '\0';
	return dup;
}

// compute the length of a null-terminated string
size_t strlen(const char *str)
{
	size_t len = 0;
	while (*str++)
		len++;

	return len;
}

char *strtok(char *str, const char *delim)
{
	// buffer to persist str between calls
	static char *save;

	if (!str)
		str = save;

	if (!str || strlen(str) == 0)
		return NULL;

	size_t delim_len = strlen(delim);

	// check if str begins with delim
	if (strncmp(str, delim, delim_len) == 0)
	{
		str += delim_len;
		if (!str)
			return NULL;
	}

	char *ret = str;

	for (; *str; str++)
	{
		if (strncmp(str, delim, delim_len) == 0)
		{
			*str = '\0';
			save = str + 1;
			return ret;
		}
	}

	save = NULL;
	return ret;
}

char *strrev(char *str)
{
	char *head = str;
	char *tail = head + strlen(head) - 1;

	for (; tail > head; head++, tail--)
	{
		// finally, an excuse to use my favorite algorithm, the in place swap
		*head ^= *tail;
		*tail ^= *head;
		*head ^= *tail;
	}

	return str;
}
