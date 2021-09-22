/******************************************************************************
 jSignal.h

	Augments signal.h

	Copyright (C) 1995-98 by John Lindal.

 ******************************************************************************/

#ifndef _H_jSignal
#define _H_jSignal

#include <signal.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (j_sig_func)(int);

#ifdef __cplusplus
}
#endif

class JString;

JString JGetSignalName(const int value);

#endif
