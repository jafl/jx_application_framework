%top {
/*
Copyright (C) 2018 by John Lindal.
*/

#include "JFunction.h"
#include <JPtrArray-JString.h>
#include "JExprParserY.hpp"
#include <jAssert.h>

#define NEW_TOKEN \
	lvalp->pString = jnew JString(text(), size()); \
	assert( lvalp->pString != nullptr );
}

%option lexer="JExprScanner"
%option lex="NextToken" params="YYSTYPE* lvalp"
%option unicode nodefault full freespace

EXP	([eE][0-9]+)

%%

%{
/************************************************************************/

	bzero(lvalp, sizeof(YYSTYPE));
%}

	/* We have to slurp e#, because that is also a valid variable name. */

0{EXP}?                  |
[1-9][0-9]*{EXP}?        |
[0-9]+"."([0-9]+)?{EXP}? |
([0-9]+)?"."[0-9]+{EXP}? {
	NEW_TOKEN
	return P_NUMBER;
}

"0"[xX][0-9a-fA-F]+ {
	NEW_TOKEN
	return P_HEX;
}

"pi"         |
"PI"         |
[\xCF][\x80] {
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

(\p{Letter}|`)(\p{Letter}|\p{Number}|`|_)* {
	NEW_TOKEN
	return P_VARIABLE;
}

[[:space:]]+	/* ignore */

	/* bison allows us to return single characters - operators included here */

. {
	return text()[0];
}

<<EOF>> {
	return P_EOF;
	}

%%
