#ifndef _H_JExprParser
#define _H_JExprParser

/******************************************************************************
 JExprParser.h

	Copyright (C) 2018 by John Lindal.

 *****************************************************************************/

#include "JExprScanner.h"

class JFunction;

class JExprParser
{
public:

	JExprParser(const JString& text);

	virtual	~JExprParser();

	int		yyparse();

	JFunction*	GetFunction() const;

private:

	JExprScanner*	itsScanner;
	JFunction*		itsCurrentNode;

private:

	int yylex(YYSTYPE* lvalp);
	int yyerror(const char* message);

	// not allowed

	JExprParser(const JExprParser& source);
	const JExprParser& operator=(const JExprParser& source);
};


/******************************************************************************
 GetRootNode

 *****************************************************************************/

inline JFunction*
JExprParser::GetFunction()
	const
{
	return itsCurrentNode;
}

#endif
