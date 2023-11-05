/******************************************************************************
 JExprParser.cpp

	Copyright (C) 2018 by John Lindal.

 *****************************************************************************/

#include "JExprParser.h"
#include "JExprScanner.h"
#include "JExprEditor.h"
#include "JSummation.h"
#include "JProduct.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

JExprParser::JExprParser
	(
	const JVariableList* varList
	)
	:
	itsEditor(nullptr),
	itsVarList(varList),
	itsParseResult(nullptr)
{
}

JExprParser::JExprParser
	(
	JExprEditor* editor
	)
	:
	itsEditor(editor),
	itsVarList(editor->GetVariableList()),
	itsParseResult(nullptr)
{
}

/******************************************************************************
 Destructor

 *****************************************************************************/

JExprParser::~JExprParser()
{
}

/******************************************************************************
 Parse

 *****************************************************************************/

bool
JExprParser::Parse
	(
	const JString&	expr,
	JFunction**		f
	)
{
	JExpr::Scanner scanner;
	itsScanner = &scanner;

	scanner.in(expr.GetBytes());

	const int result = yyparse();
	if (result == 0)
	{
		*f = itsParseResult;
	}
	else
	{
		*f = nullptr;
		jdelete itsParseResult;
	}

	itsScanner     = nullptr;
	itsParseResult = nullptr;

	return *f != nullptr;
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
	auto* s = dynamic_cast<JSummation*>(f);
	if (s != nullptr)
	{
		s->AppendArg(arg);
	}
	else
	{
		s = jnew JSummation;
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
	auto* s = dynamic_cast<JProduct*>(f);
	if (s != nullptr)
	{
		s->AppendArg(arg);
	}
	else
	{
		s = jnew JProduct;
		s->AppendArg(f);
		s->AppendArg(arg);
	}

	return s;
}
