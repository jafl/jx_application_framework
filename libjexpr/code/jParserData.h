/******************************************************************************
 jParserData.h

	Data declarations for jParserData.cc

	Copyright © 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_jParserData
#define _H_jParserData

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JDecisionType.h>
#include <JFunctionType.h>

struct JDecisionOperatorInfo
{
	JDecisionType		type;
	const JCharacter*	name;
	unsigned long		precedence;
};

struct JBoolConstInfo
{
	const JCharacter*	name;
	JBoolean			value;
};

struct JFunctionOfDiscreteInfo
{
	JFunctionType		type;
	const JCharacter*	name;
};

struct JStdFunctionInfo
{
	JFunctionType		type;
	const JCharacter*	name;
	JSize				argCount;
};
const JSize kJUnlimitedArgs = 0;

struct JNamedConstInfo
{
	const JCharacter*	name;
	JNamedConstIndex	index;
};

const JCharacter*	JPGetArgSeparatorString();
JSize				JPGetArgSeparatorStringLength();

const JCharacter*	JPGetValueUnknownString();
JSize				JPGetValueUnknownStringLength();

const JCharacter*	JPGetBooleanNOTString();
JSize				JPGetBooleanNOTStringLength();

const JCharacter*	JPGetBooleanANDString();
const JCharacter*	JPGetBooleanORString();
const JCharacter*	JPGetBooleanXORString();

JSize							JPGetBooleanOpCount();
const JDecisionOperatorInfo*	JPGetBooleanOpInfo();

const JCharacter*	JPGetEqualityString();
JSize				JPGetEqualityStringLength();
const JCharacter*	JPGetLessThanString();
const JCharacter*	JPGetLessEqualThanString();
const JCharacter*	JPGetGreaterThanString();
const JCharacter*	JPGetGreaterEqualThanString();

JSize							JPGetComparisonOpCount();
const JDecisionOperatorInfo*	JPGetComparisonOpInfo();

const JCharacter*	JPGetTrueConstDecisionString();
const JCharacter*	JPGetFalseConstDecisionString();

JSize							JPGetBoolConstCount();
const JBoolConstInfo*			JPGetBoolConstInfo();

const JCharacter*	JPGetAdditionString();
const JCharacter*	JPGetSubtractionString();
const JCharacter*	JPGetMultiplicationString();
const JCharacter*	JPGetDivisionString();
const JCharacter*	JPGetExponentString();

JSize				JPGetAdditionStringLength();
JSize				JPGetSubtractionStringLength();
JSize				JPGetMultiplicationStringLength();
JSize				JPGetDivisionStringLength();
JSize				JPGetExponentStringLength();

const JCharacter*	JPGetDiscVarValueIndexString();
const JCharacter*	JPGetDiscVarValueString();
const JCharacter*	JPGetMathematicaDiscVarValueIndexString();
const JCharacter*	JPGetMathematicaDiscVarValueString();

JSize							JPGetFnOfDiscreteCount();
const JFunctionOfDiscreteInfo*	JPGetFnOfDiscreteInfo();

JSize							JPGetStdFunctionCount();
const JStdFunctionInfo*			JPGetStdFunctionInfo();

const JCharacter*	JPGetStdFnName(const JIndex i);
const JCharacter*	JPGetMathematicaFnName(const JIndex i);

const JCharacter*	JPGetPiString();

JSize							JPGetNamedConstCount();
const JNamedConstInfo*			JPGetNamedConstInfo();

const JCharacter*	JPGetStdNamedConstName(const JIndex i);
const JCharacter*	JPGetMathematicaNamedConstName(const JIndex i);

JCharacter			JPGetGreekCharPrefixChar();
const JCharacter*	JPGetGreekCharPrefixString();
JSize				JPGetGreekCharPrefixLength();

#endif
