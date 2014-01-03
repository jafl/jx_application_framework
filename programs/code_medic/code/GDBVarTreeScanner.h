/******************************************************************************
 GDBVarTreeScanner.h

	Copyright © 1998 by Glenn W. Bach.  All rights reserved.

 *****************************************************************************/


#ifndef _H_GDBVarTreeScanner
#define _H_GDBVarTreeScanner

#ifndef _H_GDBVarTreeScannerL
#undef yyFlexLexer
#define yyFlexLexer GDBVarFlexLexer
#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <FlexLexer.h>
#endif

#ifndef NOTAB
#include "CMVarNode.h"
#include "GDBVarGroupInfo.h"
#include "GDBVarTreeParserY.hpp"
#endif

class GDBVarTreeScanner : public GDBVarFlexLexer
{
public:

	GDBVarTreeScanner(const JCharacter* text);

	virtual	~GDBVarTreeScanner();

	int yylex(YYSTYPE* lvalp);

protected:

	virtual size_t LexerInput(char* buf, size_t max_size);

private:

	JString	itsInputBuffer;
	JSize	itsBytesRead;
	JString	itsString;
	JSize	itsBracketCount;

private:

	// not allowed

	GDBVarTreeScanner(const GDBVarTreeScanner& source);
	const GDBVarTreeScanner& operator=(const GDBVarTreeScanner& source);
};

#endif
