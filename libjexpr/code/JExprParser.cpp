/******************************************************************************
 JExprParser.cpp

	Copyright (C) 2018 by John Lindal.

 *****************************************************************************/

#include "JExprParser.h"
#include "JSummation.h"
#include "JProduct.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

JExprParser::JExprParser
	(
	const JString&			text,
	const JVariableList*	varList,
	JFontManager*			fontManager,
	const JBoolean			allowUIF
	)
	:
	itsVarList(varList),
	itsFontManager(fontManager),
	itsAllowUIFFlag(allowUIF),
	itsCurrentNode(nullptr)
{
	itsScanner = jnew JExprScanner(text);
	assert( itsScanner != nullptr );
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

/******************************************************************************
 UpdateSum (private)

 *****************************************************************************/

JFunction*
JExprParser::UpdateSum
	(
	JFunction*	f,
	JFunction*	arg
	)
{
	JSummation* s = dynamic_cast<JSummation*>(f);
	if (s != nullptr)
		{
		s->AppendArg(arg);
		}
	else
		{
		s = jnew JSummation;
		assert( s != nullptr );
		s->AppendArg(f);
		s->AppendArg(arg);
		}

	return s;
}

/******************************************************************************
 UpdateProduct (private)

 *****************************************************************************/

JFunction*
JExprParser::UpdateProduct
	(
	JFunction*	f,
	JFunction*	arg
	)
{
	JProduct* s = dynamic_cast<JProduct*>(f);
	if (s != nullptr)
		{
		s->AppendArg(arg);
		}
	else
		{
		s = jnew JProduct;
		assert( s != nullptr );
		s->AppendArg(f);
		s->AppendArg(arg);
		}

	return s;
}
