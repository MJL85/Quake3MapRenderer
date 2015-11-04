/**
 *	@file
 *	@brief General definitions.
 */

#ifndef DEFINITIONS_H_INCLUDED
#define DEFINITIONS_H_INCLUDED

typedef unsigned char byte;

/* uncomment the following line to enable debugging */
#define _DEBUG_

/** Error output macro. */
#define ERROR(fmt, ...)		fprintf(stderr, "[ERROR] " __FILE__ ":%i: " fmt "\n",	\
									__LINE__, ##__VA_ARGS__)

/** Warning output macro. */
#define WARNING(fmt, ...)	fprintf(stdout, "[WARNING] " __FILE__ ":%i: " fmt "\n",	\
									__LINE__, ##__VA_ARGS__)

/** Information output macro. */
#define INFO(fmt, ...)		fprintf(stdout, "[INFO] " fmt "\n", ##__VA_ARGS__)

#ifdef _DEBUG_
	/** Debug output macro. */
	#define DEBUG(fmt, ...)	fprintf(stdout, "[DEBUG] " __FILE__ ":%i: " fmt "\n",	\
									__LINE__, ##__VA_ARGS__)
#endif


#ifdef _WIN32
	#define OS_NAME			"Windows"
	#define OS_DIR_DELIM	'\\'
#else
	#define OS_NAME			"Linux"
	#define OS_DIR_DELIM	'/'
#endif

#define TB_INLINE			inline

#define TB_VERSION			"0.1dev"
#define STARTUP_BANNER		"Typeball v" TB_VERSION " (" OS_NAME ") Compiled on " __DATE__ " at " __TIME__ "."

#endif // DEFINITIONS_H_INCLUDED
