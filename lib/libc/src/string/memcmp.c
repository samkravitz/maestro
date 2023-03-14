#include <string.h>

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
