/******************************************************************************
 JExprScanner.h

	Copyright (C) 2018 by John Lindal.

 *****************************************************************************/

#ifndef _H_JExprScanner
#define _H_JExprScanner

#ifndef _H_JExprScannerL
#undef yyFlexLexer
#define yyFlexLexer JExprFlexLexer
#include <FlexLexer.h>
#endif

#ifndef NOTAB
#include "JFunction.h"
#include <JPtrArray-JString.h>
#include "JExprParserY.hpp"
#endif

#include <JFlexLexerHacks.h>

class JExprScanner : public JExprFlexLexer
{
public:

	JExprScanner(const JString& text);

	virtual	~JExprScanner();

	int NextToken(YYSTYPE* lvalp);

protected:

	virtual FlexLexerInputSize LexerInput(char* buf, FlexLexerInputSize max_size) override;

private:

	JString	itsInputBuffer;
	JSize	itsBytesRead;

private:

	// not allowed

	JExprScanner(const JExprScanner& source);
	const JExprScanner& operator=(const JExprScanner& source);
};

#endif
