#ifndef _H_JExprParser
#define _H_JExprParser

/******************************************************************************
 JExprParser.h

	Copyright (C) 2018 by John Lindal.

 *****************************************************************************/

#include "JExprScanner.h"

class JFunction;
class JFontManager;

class JExprParser
{
public:

	JExprParser(const JString& text, const JVariableList* varList,
				JFontManager* fontManager, const JBoolean allowUIF = kJFalse);

	virtual	~JExprParser();

	int		yyparse();

	JFunction*	GetFunction() const;

private:

	const JVariableList*	itsVarList;
	JFontManager*			itsFontManager;
	const JBoolean			itsAllowUIFFlag;

	JExprScanner*	itsScanner;
	JFunction*		itsCurrentNode;

private:

	int yylex(YYSTYPE* lvalp);
	int yyerror(const char* message);

	JFunction*	UpdateSum(JFunction* f, JFunction* arg);
	JFunction*	UpdateProduct(JFunction* f, JFunction* arg);

	// not allowed

	JExprParser(const JExprParser& source);
	const JExprParser& operator=(const JExprParser& source);
};


/******************************************************************************
 GetFunction

 *****************************************************************************/

inline JFunction*
JExprParser::GetFunction()
	const
{
	return itsCurrentNode;
}

#endif
