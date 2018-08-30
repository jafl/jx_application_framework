/******************************************************************************
 JExprParser.cpp

	Copyright (C) 2018 by John Lindal.

 *****************************************************************************/

#include "JExprParser.h"
#include "JFunction.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

JExprParser::JExprParser
	(
	const JString& text
	)
	:
	itsCurrentNode(NULL)
{
	itsScanner = jnew JExprScanner(text);
	assert(itsScanner != NULL);
}

/******************************************************************************
 Destructor

 *****************************************************************************/

JExprParser::~JExprParser()
{
	jdelete itsScanner;
	jdelete itsCurrentNode;
}

/******************************************************************************
 yylex (private)

 *****************************************************************************/

int
JExprParser::yylex
	(
	YYSTYPE* lvalp
	)
{
	return itsScanner->NextToken(lvalp);
}

/******************************************************************************
 yyerror (private)

 *****************************************************************************/

int
JExprParser::yyerror
	(
	const char* message
	)
{
	std::cerr << "yyerror() called: " << message << std::endl;
	return 0;
}
