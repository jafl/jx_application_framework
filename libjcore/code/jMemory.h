/******************************************************************************
 jMemory.h

	Interface for jMemory.cc

	Copyright © 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_jMemory
#define _H_jMemory

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <jTypes.h>

JCharacter* JCreateBuffer(JSize* bufferSize);

#endif
