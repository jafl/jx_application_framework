/******************************************************************************
 jTextUtil.h

   Copyright (C) 2017 by John Lindal. All rights reserved.

 *****************************************************************************/

#ifndef _H_jTextUtil
#define _H_jTextUtil

#include "JString.h"

void JAnalyzeWhitespace(const JString& buffer, const JSize tabWidth,
						const JBoolean defaultUseSpaces,
						JBoolean* useSpaces, JBoolean* isMixed);

#endif
