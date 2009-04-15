/******************************************************************************
 jCommandLine.h

	Interface for jCommandLine.cc

	Copyright © 1994-99 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_jCommandLine
#define _H_jCommandLine

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <jTypes.h>

void JWaitForReturn();
void JInputFinished();

void		JCheckForValues(const JSize valueCount, JIndex* index,
							const JSize argc, char* argv[]);
JBoolean	JIsVersionRequest(const JCharacter* arg);
JBoolean	JIsHelpRequest(const JCharacter* arg);

#endif
