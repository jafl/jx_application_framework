%{
/*
Copyright (C) 2018 by John Lindal.
*/

#include "jx-af/jexpr/JAbsValue.h"
#include "jx-af/jexpr/JAlgSign.h"
#include "jx-af/jexpr/JArcCosine.h"
#include "jx-af/jexpr/JArcHypCosine.h"
#include "jx-af/jexpr/JArcHypSine.h"
#include "jx-af/jexpr/JArcHypTangent.h"
#include "jx-af/jexpr/JArcSine.h"
#include "jx-af/jexpr/JArcTangent.h"
#include "jx-af/jexpr/JArcTangent2.h"
#include "jx-af/jexpr/JConjugate.h"
#include "jx-af/jexpr/JConstantValue.h"
#include "jx-af/jexpr/JCosine.h"
#include "jx-af/jexpr/JDivision.h"
#include "jx-af/jexpr/JExponent.h"
#include "jx-af/jexpr/JHypCosine.h"
#include "jx-af/jexpr/JHypSine.h"
#include "jx-af/jexpr/JHypTangent.h"
#include "jx-af/jexpr/JImagPart.h"
#include "jx-af/jexpr/JLogB.h"
#include "jx-af/jexpr/JLogE.h"
#include "jx-af/jexpr/JMaxFunc.h"
#include "jx-af/jexpr/JMinFunc.h"
#include "jx-af/jexpr/JNamedConstant.h"
#include "jx-af/jexpr/JNegation.h"
#include "jx-af/jexpr/JParallel.h"
#include "jx-af/jexpr/JPhaseAngle.h"
#include "jx-af/jexpr/JProduct.h"
#include "jx-af/jexpr/JRealPart.h"
#include "jx-af/jexpr/JRotateComplex.h"
#include "jx-af/jexpr/JRoundToInt.h"
#include "jx-af/jexpr/JSine.h"
#include "jx-af/jexpr/JSquareRoot.h"
#include "jx-af/jexpr/JSummation.h"
#include "jx-af/jexpr/JTangent.h"
#include "jx-af/jexpr/JTruncateToInt.h"
#include "jx-af/jexpr/JUserInputFunction.h"
#include "jx-af/jexpr/JVariableValue.h"
#include "jx-af/jexpr/JVariableList.h"
#include <jx-af/jcore/JPtrArray-JString.h>

// also uncomment yydebug=1; below
//#define YYERROR_VERBOSE
//#define YYDEBUG 1
%}

%union {
	JString*				pString;
	JFunction*				pFunction;
	JPtrArray<JFunction>*	pList;
}

%{
#define NOTAB
#include "jx-af/jexpr/JExprParser.h"

#define yyparse JExprParser::yyparse
%}

%define api.pure

%token<pString>	P_NUMBER P_HEX P_VARIABLE
%token			P_PI P_E P_I
%token			P_INPUT
%token			P_FN_ABS P_FN_ARCCOS P_FN_ARCCOSH P_FN_ARCSIN P_FN_ARCSINH
%token			P_FN_ARCTAN P_FN_ARCTAN2 P_FN_ARCTANH P_FN_CONJUGATE
%token			P_FN_COS P_FN_COSH P_FN_IM P_FN_LN P_FN_LOG P_FN_LOG2
%token			P_FN_LOG10 P_FN_MAX P_FN_MIN P_FN_PHASE P_FN_RE
%token			P_FN_ROTATE P_FN_SIN P_FN_SINH P_FN_SQRT P_FN_TAN P_FN_TANH
%token			P_FN_SIGN P_FN_ROUND P_FN_TRUNCATE P_FN_PARALLEL
%token			P_EOF

%type<pFunction>	expression e
%type<pList>		arglist

%{

// debugging output
/*
#define YYPRINT(file, type, value)	yyprint(file, type, value)
#include <stdio.h>

inline void
yyprint
	(
	FILE*	file,
	int		type,
	YYSTYPE	value
	)
{
	if (value.pString != nullptr)
	{
		fprintf(file, "string:  %s", (value.pString)->GetBytes());
	}
}
*/
%}

%{
#include <jx-af/jcore/jAssert.h>	// must be last
%}

%initial-action
{
//	yydebug = 1;
}

/* operator associations and precedence */

%left '+' '-'
%left '*' '/'
%right '^'
%left UMINUS UPLUS

%%

expression
	: e P_EOF
	{
		itsParseResult = $1;
		YYACCEPT;
	}
	;

e
	: P_NUMBER
	{
		JPtrArray<JString> s(JPtrArrayT::kDeleteAll);
		$1->ToLower();
		$1->Split("e", &s, 2);

		if (s.GetElementCount() == 2)
		{
			JFloat v, e;
			if (!s.GetElement(1)->ConvertToFloat(&v) ||
				!s.GetElement(2)->ConvertToFloat(&e))
			{
				YYERROR;
			}
			JProduct* p = jnew JProduct();
			assert( p != nullptr );
			p->AppendArg(jnew JConstantValue(v));
			p->AppendArg(jnew JExponent(jnew JConstantValue(10), jnew JConstantValue(e)));
			$$ = p;
		}
		else
		{
			JFloat v;
			if (!$1->ConvertToFloat(&v))
			{
				YYERROR;
			}
			$$ = jnew JConstantValue(v);
			assert( $$ != nullptr );
		}
	}

	| P_NUMBER P_E '+' P_NUMBER
	{
		JFloat v, e;
		if (!$1->ConvertToFloat(&v) || !$4->ConvertToFloat(&e))
		{
			YYERROR;
		}
		JProduct* p = jnew JProduct();
		assert( p != nullptr );
		p->AppendArg(jnew JConstantValue(v));
		p->AppendArg(jnew JExponent(jnew JConstantValue(10), jnew JConstantValue(e)));
		$$ = p;
	}

	| P_NUMBER P_E '-' P_NUMBER
	{
		JFloat v, e;
		if (!$1->ConvertToFloat(&v) || !$4->ConvertToFloat(&e))
		{
			YYERROR;
		}
		JProduct* p = jnew JProduct();
		assert( p != nullptr );
		p->AppendArg(jnew JConstantValue(v));
		p->AppendArg(jnew JExponent(jnew JConstantValue(10), jnew JNegation(jnew JConstantValue(e))));
		$$ = p;
	}

	| P_HEX
	{
		JFloat v;
		if (!$1->ConvertToFloat(&v))
		{
			YYERROR;
		}
		$$ = jnew JConstantValue(v);
		assert( $$ != nullptr );
	}

	| P_E
	{
		$$ = jnew JNamedConstant(kEJNamedConstIndex);
		assert( $$ != nullptr );
	}

	| P_PI
	{
		$$ = jnew JNamedConstant(kPiJNamedConstIndex);
		assert( $$ != nullptr );
	}

	| P_I
	{
		$$ = jnew JNamedConstant(kIJNamedConstIndex);
		assert( $$ != nullptr );
	}

	| P_VARIABLE
	{
		JIndex i;
		if (!itsVarList->ParseVariableName(*$1, &i))
		{
			YYERROR;
		}
		$$ = jnew JVariableValue(itsVarList, i, nullptr);
		assert( $$ != nullptr );
	}

	| P_VARIABLE '[' e ']'
	{
		JIndex i;
		if (!itsVarList->ParseVariableName(*$1, &i))
		{
			YYERROR;
		}
		$$ = jnew JVariableValue(itsVarList, i, $3);
		assert( $$ != nullptr );
	}

	| P_INPUT
	{
		if (itsEditor != nullptr)
		{
			$$ = jnew JUserInputFunction(itsEditor);
		}
		else
		{
			YYERROR;
		}
		assert( $$ != nullptr );
	}

	| '(' e ')'
	{
		$$ = $2;
	}

	| e '+' e
	{
		$$ = UpdateSum($1, $3);
	}

	| e '-' e
	{
		$$ = UpdateSum($1, jnew JNegation($3));
	}

	| e '*' e
	{
		$$ = UpdateProduct($1, $3);
	}

	| e '/' e
	{
		$$ = jnew JDivision($1, $3);
		assert( $$ != nullptr );
	}

	| e '^' e
	{
		$$ = jnew JExponent($1, $3);
		assert( $$ != nullptr );
	}

	| '-' e %prec UMINUS
	{
		$$ = jnew JNegation($2);
		assert( $$ != nullptr );
	}

	| '+' e %prec UPLUS
	{
		$$ = $2;
	}

	| P_FN_ABS '(' e ')'
	{
		$$ = jnew JAbsValue($3);
		assert( $$ != nullptr );
	}

	| P_FN_PHASE '(' e ')'
	{
		$$ = jnew JPhaseAngle($3);
		assert( $$ != nullptr );
	}

	| P_FN_CONJUGATE '(' e ')'
	{
		$$ = jnew JConjugate($3);
		assert( $$ != nullptr );
	}

	| P_FN_ROTATE '(' e ',' e ')'
	{
		$$ = jnew JRotateComplex($3, $5);
		assert( $$ != nullptr );
	}

	| P_FN_RE '(' e ')'
	{
		$$ = jnew JRealPart($3);
		assert( $$ != nullptr );
	}

	| P_FN_IM '(' e ')'
	{
		$$ = jnew JImagPart($3);
		assert( $$ != nullptr );
	}

	| P_FN_MIN '(' arglist ')'
	{
		$$ = jnew JMinFunc($3);
		assert( $$ != nullptr );
	}

	| P_FN_MAX '(' arglist ')'
	{
		$$ = jnew JMaxFunc($3);
		assert( $$ != nullptr );
	}

	| P_FN_SQRT '(' e ')'
	{
		$$ = jnew JSquareRoot($3);
		assert( $$ != nullptr );
	}

	| P_FN_PARALLEL '(' arglist ')'
	{
		$$ = jnew JParallel($3);
		assert( $$ != nullptr );
	}

	| P_FN_SIGN '(' e ')'
	{
		$$ = jnew JAlgSign($3);
		assert( $$ != nullptr );
	}

	| P_FN_ROUND '(' e ')'
	{
		$$ = jnew JRoundToInt($3);
		assert( $$ != nullptr );
	}

	| P_FN_TRUNCATE '(' e ')'
	{
		$$ = jnew JTruncateToInt($3);
		assert( $$ != nullptr );
	}

	| P_FN_LOG '(' e ',' e ')'
	{
		$$ = jnew JLogB($3, $5);
		assert( $$ != nullptr );
	}

	| P_FN_LOG2 '(' e ')'
	{
		$$ = jnew JLogB(jnew JConstantValue(2), $3);
		assert( $$ != nullptr );
	}

	| P_FN_LOG10 '(' e ')'
	{
		$$ = jnew JLogB(jnew JConstantValue(10), $3);
		assert( $$ != nullptr );
	}

	| P_FN_LN '(' e ')'
	{
		$$ = jnew JLogE($3);
		assert( $$ != nullptr );
	}

	| P_FN_ARCSIN '(' e ')'
	{
		$$ = jnew JArcSine($3);
		assert( $$ != nullptr );
	}

	| P_FN_ARCCOS '(' e ')'
	{
		$$ = jnew JArcCosine($3);
		assert( $$ != nullptr );
	}

	| P_FN_ARCTAN '(' e ')'
	{
		$$ = jnew JArcTangent($3);
		assert( $$ != nullptr );
	}

	| P_FN_ARCTAN2 '(' e ',' e ')'
	{
		$$ = jnew JArcTangent2($3, $5);
		assert( $$ != nullptr );
	}

	| P_FN_SIN '(' e ')'
	{
		$$ = jnew JSine($3);
		assert( $$ != nullptr );
	}

	| P_FN_COS '(' e ')'
	{
		$$ = jnew JCosine($3);
		assert( $$ != nullptr );
	}

	| P_FN_TAN '(' e ')'
	{
		$$ = jnew JTangent($3);
		assert( $$ != nullptr );
	}

	| P_FN_SINH '(' e ')'
	{
		$$ = jnew JHypSine($3);
		assert( $$ != nullptr );
	}

	| P_FN_COSH '(' e ')'
	{
		$$ = jnew JHypCosine($3);
		assert( $$ != nullptr );
	}

	| P_FN_TANH '(' e ')'
	{
		$$ = jnew JHypTangent($3);
		assert( $$ != nullptr );
	}

	| P_FN_ARCSINH '(' e ')'
	{
		$$ = jnew JArcHypSine($3);
		assert( $$ != nullptr );
	}

	| P_FN_ARCCOSH '(' e ')'
	{
		$$ = jnew JArcHypCosine($3);
		assert( $$ != nullptr );
	}

	| P_FN_ARCTANH '(' e ')'
	{
		$$ = jnew JArcHypTangent($3);
		assert( $$ != nullptr );
	}
	;

arglist
	: e
	{
		$$ = jnew JPtrArray<JFunction>(JPtrArrayT::kDeleteAll);
		assert( $$ != nullptr );
		$$->AppendElement($1);
	}

	| arglist ',' e
	{
		$$ = $1; $$->AppendElement($3);
	}
	;
