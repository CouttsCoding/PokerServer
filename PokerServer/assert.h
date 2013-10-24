#ifndef ASSERT_H
#define ASSERT_H

/////////////////////////////////////////////////////////
// Sanity checking for the programmer. Crash your program
// deliberately if an assert fails!
// Author: Mark Mikulec, 2010.

#include <assert.h>
#include <stdlib.h>

#pragma warning(disable : 4127)

#ifdef _WIN32
#define DoAssert(expression)		{ if(!(expression)) { do { __asm int 3 } while(0); } }
#else
#define DoAssert(expression)		{ if(!(expression)) { fprintf(stderr, __FILE__ ":%d assert failed.", __LINE__); fflush(stderr); do { __asm__ __volatile__ ("int $0x03"); } while(0); } }
#endif

#ifdef _DEBUG	
	#define DebugAssert(expression) DoAssert(expression)
	#define DebugVerify(expression) DoAssert(expression)
#else                                                   
    #define //DebugAssert(expression) { }
	#define DebugVerify(expression) { if(expression){} }
#endif

#define Assert(expression) DoAssert(expression)

#endif//ASSERT_H
