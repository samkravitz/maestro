#include "string.h"

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
