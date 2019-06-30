/******************************************************************************
 CMFlexLexerHacks.h

	flex uses a different argument type on OSX and Linux :(

	Copyright (C) 2014 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CMFlexLexerHacks
#define _H_CMFlexLexerHacks

#ifdef _J_OSX
typedef size_t FlexLexerInputSize;
#else
typedef int FlexLexerInputSize;
#endif

#endif
