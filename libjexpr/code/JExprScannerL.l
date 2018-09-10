%{
/*
Copyright (C) 2018 by John Lindal.
*/

#define _H_JExprScannerL

#include "JExprScanner.h"
#include <jAssert.h>

#undef YY_DECL
#define YY_DECL int JExprScanner::NextToken(YYSTYPE* lvalp)

#define NEW_TOKEN(s) \
	lvalp->pString = jnew JString(s); \
	assert( lvalp->pString != nullptr );
%}

%option c++ yyclass = "JExprScanner" prefix="JExpr"
%option 8bit nodefault
%option noyywrap

EXP	([eE][0-9]+)

%%

%{
/************************************************************************/

	bzero(lvalp, sizeof(YYSTYPE));
%}

	/* We have to slurp e#, because that is also a valid variable name. */

0{EXP}?                   |
[1-9][0-9]*{EXP}?         |
[0-9]+"."([0-9]+)?{EXP}?  |
([0-9]+)?"."[0-9]+{EXP}?  |
"0"[xX][0-9a-fA-F]+{EXP}? {
	NEW_TOKEN(yytext);
	return P_NUMBER;
}

"pi"     |
"PI"     |
"\u03c0" {
	return P_PI;
	}

[eE] {
	return P_E;
	}

[iIjJ] {
	return P_I;
	}

"?" {
	return P_INPUT;
	}

"abs" {
	return P_FN_ABS;
	}

"arccos" {
	return P_FN_ARCCOS;
	}

"arccosh" {
	return P_FN_ARCCOSH;
	}

"arcsin" {
	return P_FN_ARCSIN;
	}

"arcsinh" {
	return P_FN_ARCSINH;
	}

"arctan" {
	return P_FN_ARCTAN;
	}

"arctan2" {
	return P_FN_ARCTAN2;
	}

"arctanh" {
	return P_FN_ARCTANH;
	}

"conjugate" {
	return P_FN_CONJUGATE;
	}

"cos" {
	return P_FN_COS;
	}

"cosh" {
	return P_FN_COSH;
	}

"im" {
	return P_FN_IM;
	}

"ln" {
	return P_FN_LN;
	}

"log" {
	return P_FN_LOG;
	}

"log2" {
	return P_FN_LOG2;
	}

"log10" {
	return P_FN_LOG10;
	}

"max" {
	return P_FN_MAX;
	}

"min" {
	return P_FN_MIN;
	}

"parallel" {
	return P_FN_PARALLEL;
	}

"arg" {
	return P_FN_PHASE;
	}

"re" {
	return P_FN_RE;
	}

"rotate" {
	return P_FN_ROTATE;
	}

"round" {
	return P_FN_ROUND;
	}

"sign" {
	return P_FN_SIGN;
	}

"sin" {
	return P_FN_SIN;
	}

"sinh" {
	return P_FN_SINH;
	}

"sqrt" {
	return P_FN_SQRT;
	}

"tan" {
	return P_FN_TAN;
	}

"tanh" {
	return P_FN_TANH;
	}

"truncate" {
	return P_FN_TRUNCATE;
	}

[[:alpha:]`][[:alnum:]`_]* {
	NEW_TOKEN(yytext);
	return P_VARIABLE;
}

[[:space:]]+	/* ignore */

	/* bison allows us to return single characters - operators included here */

. return yytext[0];

<<EOF>> {
	return P_EOF;
	}

%%
