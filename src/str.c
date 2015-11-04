#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "str.h"

/**
 *	@brief Case insensitive string compare.
 */
int cstrcmp(char* s1, char* s2) {
	if (!s1 && s2)			return -1;
	else if (s1 && !s2)		return 1;
	else if (!s1 && !s2)	return 0;

	while (*s1) {
		if (!*s2)
			return 1;
		if (tolower(*s1) > tolower(*s2))
			return 1;
		else if (tolower(*s1) < tolower(*s2))
			return -1;
		++s1;
		++s2;
	}

	return (*s2 ? -1 : 0);
}
