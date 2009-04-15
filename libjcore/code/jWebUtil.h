/******************************************************************************
 jWebUtil.h

	Utility functions for dealing with URLs.

	Copyright © 2008 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_jWebUtil
#define _H_jWebUtil

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <jTypes.h>

JBoolean	JIsURL(const JCharacter* s);

#endif
