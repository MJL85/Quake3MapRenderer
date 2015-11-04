#ifndef STR_H_INCLUDED
#define STR_H_INCLUDED

/**
 *	@file str.h
 *	@brief String related functions
 */

#define IS_ALPHA(x)				(((x >= 'a') && (x <= 'z')) || ((x >= 'A') && (x <= 'Z')))
#define IS_NUMERIC(x)			((x >= '0') && (x <= '9'))
#define IS_ALPHANUMERIC(x)		(IS_ALPHA(x) && IS_NUMERIC(x))


#ifdef __cplusplus
extern "C"
{
#endif

int cstrcmp(char* s1, char* s2);

#ifdef __cplusplus
}
#endif


#endif // STR_H_INCLUDED
