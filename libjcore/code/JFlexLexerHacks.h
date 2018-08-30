/******************************************************************************
 JFlexLexerHacks.h

	flex uses a different argument type on OSX and Linux :(

	Copyright (C) 2014 by John Lindal.

 ******************************************************************************/

#ifndef _H_JFlexLexerHacks
#define _H_JFlexLexerHacks

#ifdef _J_OSX
typedef size_t FlexLexerInputSize;
#else
typedef int FlexLexerInputSize;
#endif

#endif
