/******************************************************************************
 jCommandLine.h

	Interface for jCommandLine.cc

	Copyright (C) 1994-99 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_jCommandLine
#define _H_jCommandLine

#include <jTypes.h>

void JWaitForReturn();
void JInputFinished();

void		JCheckForValues(const JSize valueCount, JIndex* index,
							const JSize argc, char* argv[]);
JBoolean	JIsVersionRequest(const JCharacter* arg);
JBoolean	JIsHelpRequest(const JCharacter* arg);

#endif
