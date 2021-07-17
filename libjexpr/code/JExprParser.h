#ifndef _H_JExprParser
#define _H_JExprParser

/******************************************************************************
 JExprParser.h

	Copyright (C) 2018 by John Lindal.

 *****************************************************************************/

#include "JExprScanner.h"

class JFunction;
class JFontManager;
class JExprEditor;

class JExprParser
{
public:

	JExprParser(const JVariableList* varList);
	JExprParser(JExprEditor* editor);

	virtual	~JExprParser();

	bool	Parse(const JString& expr, JFunction** f);

private:

	JExprEditor*			itsEditor;		// if not NULL, enables JUserInputFunction
	const JVariableList*	itsVarList;

	JExprScanner*	itsScanner;
	JFunction*		itsParseResult;

private:

	int yyparse();
	int yylex(YYSTYPE* lvalp);
	int yyerror(const char* message);

	JFunction*	UpdateSum(JFunction* f, JFunction* arg);
	JFunction*	UpdateProduct(JFunction* f, JFunction* arg);

	// not allowed

	JExprParser(const JExprParser& source);
	const JExprParser& operator=(const JExprParser& source);
};

#endif
