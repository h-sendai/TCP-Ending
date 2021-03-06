#ifndef LOG_UTIL
#define LOG_UTIL 1

#include <sys/time.h>

#include <err.h>
#include <stdarg.h>
#include <stdio.h>
#include <time.h>

extern int debug;
extern int fprintfwt(FILE *pFILE,const char *fmt,...);
extern int debug_print(FILE *pFILE,const char *fmt,...);
extern void errwt(int eval, const char *fmt, ...);

/*************************************************************************
	System : KEK MLF Module Tester.
	Author : wada@bbtech.co.jp
	Description : BBT MLF Utilites for Linux
	$KMM_TESTER_BEGIN_LICENSE$
	$KMM_TESTER_END_LICENSE$
*************************************************************************/

/****************************************************************
 *fprintf with time - for logging function
 *****************************************************************/

#endif
