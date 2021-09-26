/******************************************************************************
 jCommandLine.h

	Copyright (C) 1994-99 by John Lindal.

 ******************************************************************************/

#ifndef _H_jCommandLine
#define _H_jCommandLine

#include "jx-af/jcore/jTypes.h"

void JWaitForReturn();
void JInputFinished();

void		JCheckForValues(const JSize valueCount, JIndex* index,
							const JSize argc, char* argv[]);
bool	JIsVersionRequest(const JUtf8Byte* arg);
bool	JIsHelpRequest(const JUtf8Byte* arg);

#endif
