/******************************************************************************
 jParseDecision.cpp

							The JDecision Parser Module

	This module parses a string into a JDecision.

	Copyright (C) 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <jParseDecision.h>
#include <jParseDecision.ph>
#include <jParseFunction.ph>
#include <jParseUtil.h>
#include <jParserData.h>
#include <JParseResult.h>

#include <JDecisionType.h>
#include <JBooleanAND.h>
#include <JBooleanOR.h>
#include <JBooleanXOR.h>
#include <JBooleanNOT.h>
#include <JBooleanConstant.h>
#include <JValueUnknown.h>
#include <JDiscreteEquality.h>
#include <JDiscreteVarEquality.h>
#include <JFunctionEquality.h>
#include <JLessThan.h>
#include <JLessEqualThan.h>
#include <JGreaterThan.h>
#include <JGreaterEqualThan.h>

#include <JVariableList.h>
#include <JFunction.h>

#include <JString.h>
#include <ctype.h>
#include <jGlobals.h>
#include <jAssert.h>

// Private declarations

JBoolean
JIsBoolConstant(const JCharacter* origExpr, const JSize origLength,
				JDecision** theDecision);
JParseResult
JParseAsNegation(const JCharacter* origExpr, const JSize origLength,
				 const JVariableList* theVariableList, JDecision** theDecision);
JParseResult
JParseAsUnknown(const JCharacter* origExpr, const JSize origLength,
				const JVariableList* theVariableList, JDecision** theDecision);
JParseResult
JParseAsDiscreteComparison(const JCharacter* origExpr, const JSize origLength,
						   const JVariableList* theVariableList, JDecision** theDecision);

JBoolean JContainsBooleanOperator(const JCharacter* expr, const JSize length,
								  JDecisionType* type, JSize* start, JSize* end);
JBoolean JContainsComparisonOperator(const JCharacter* expr, const JSize length,
									 JDecisionType* type, JSize* start, JSize* end);
JBoolean JFindLowestPrecedenceOperator(
			const JCharacter* expr, const JSize length,
			JDecisionType* type, JSize* start, JSize* end,
			const JSize opCount, const JDecisionOperatorInfo op[]);

/******************************************************************************
 JParseDecision

	The main entry point for this module.  It initiates the recursive parsing
	of expr into a JDecision.

 ******************************************************************************/

JBoolean
JParseDecision
	(
	const JCharacter*		expr,
	const JVariableList*	theVariableList,
	JDecision**				theDecision
	)
{
	return JRecurseDecision(expr, strlen(expr), theVariableList, theDecision);
}

/******************************************************************************
 JRecurseDecision

	Parse the indicated portion of the string into a JDecision.

 ******************************************************************************/

JBoolean
JRecurseDecision
	(
	const JCharacter*		origExpr,
	const JSize				origLength,
	const JVariableList*	theVariableList,
	JDecision**				theDecision
	)
{
	*theDecision = NULL;

	// remove enclosing parentheses

	const JCharacter* expr = origExpr;
	const JSize length     = JStripParentheses(&expr, origLength);

	if (length == 0)
		{
		(JGetUserNotification())->ReportError("You specified an empty decision");
		return kJFalse;
		}

	// check for a constant

	if (JIsBoolConstant(expr, length, theDecision))
		{
		return kJTrue;
		}

	// search for an operator

	JDecisionType type;
	JSize start,end;

	if (JContainsBooleanOperator(expr, length, &type, &start, &end))
		{
		JDecision* arg1 = NULL;
		JDecision* arg2 = NULL;

		if (!JRecurseDecision(expr, start, theVariableList, &arg1) ||
			!JRecurseDecision(expr + end, length - end, theVariableList, &arg2))
			{
			jdelete arg1;
			jdelete arg2;
			return kJFalse;
			}

		if (type == kJBooleanANDType)
			{
			*theDecision = jnew JBooleanAND(arg1, arg2);
			}
		else if (type == kJBooleanORType)
			{
			*theDecision = jnew JBooleanOR(arg1, arg2);
			}
		else if (type == kJBooleanXORType)
			{
			*theDecision = jnew JBooleanXOR(arg1, arg2);
			}
		else
			{
			cerr << "JRecurseDecision:  unknown boolean operator" << endl;
			jdelete arg1;
			jdelete arg2;
			return kJFalse;
			}
		assert( *theDecision != NULL );
		return kJTrue;
		}

	// check for negation

	JParseResult result = JParseAsNegation(expr, length, theVariableList, theDecision);
	if (result == kJParsedOK)
		{
		return kJTrue;
		}
	else if (result == kJParseError)
		{
		return kJFalse;
		}

	// check for "is unknown"

	result = JParseAsUnknown(expr, length, theVariableList, theDecision);
	if (result == kJParsedOK)
		{
		return kJTrue;
		}
	else if (result == kJParseError)
		{
		return kJFalse;
		}

	// check for discrete variable

	result = JParseAsDiscreteComparison(expr, length, theVariableList, theDecision);
	if (result == kJParsedOK)
		{
		return kJTrue;
		}
	else if (result == kJParseError)
		{
		return kJFalse;
		}

	// check for numeric comparison

	if (JContainsComparisonOperator(expr, length, &type, &start, &end))
		{
		JFunction* arg1 = NULL;
		JFunction* arg2 = NULL;

		if (!JRecurseFunction(expr, start, theVariableList, &arg1) ||
			!JRecurseFunction(expr + end, length - end, theVariableList, &arg2))
			{
			jdelete arg1;
			jdelete arg2;
			return kJFalse;
			}

		if (type == kJFunctionEqualityType)
			{
			*theDecision = jnew JFunctionEquality(arg1, arg2);
			}
		else if (type == kJLessThanType)
			{
			*theDecision = jnew JLessThan(arg1, arg2);
			}
		else if (type == kJLessEqualThanType)
			{
			*theDecision = jnew JLessEqualThan(arg1, arg2);
			}
		else if (type == kJGreaterThanType)
			{
			*theDecision = jnew JGreaterThan(arg1, arg2);
			}
		else if (type == kJGreaterEqualThanType)
			{
			*theDecision = jnew JGreaterEqualThan(arg1, arg2);
			}
		else
			{
			cerr << "JRecurseDecision:  unknown comparison operator" << endl;
			jdelete arg1;
			jdelete arg2;
			return kJFalse;
			}
		assert( *theDecision != NULL );
		return kJTrue;
		}

	// this string is not a decision

	JString errorStr(expr, length);
	errorStr.Prepend("\"");
	errorStr += "\" is not a valid boolean expression";
	(JGetUserNotification())->ReportError(errorStr);
	return kJFalse;
}

/******************************************************************************
 JIsBoolConstant

	Syntax:  "True" or "False"

 ******************************************************************************/

JBoolean
JIsBoolConstant
	(
	const JCharacter*	origExpr,
	const JSize			origLength,
	JDecision**			theDecision
	)
{
	*theDecision = NULL;

	// remove enclosing parentheses

	const JCharacter* expr = origExpr;
	const JSize length     = JStripParentheses(&expr, origLength);

	// check if expr is a known constant

	const JSize boolCount          = JPGetBoolConstCount();
	const JBoolConstInfo* boolInfo = JPGetBoolConstInfo();

	for (JSize i=1; i<=boolCount; i++)
		{
		if (JStringsEqual(expr, length, boolInfo[i-1].name))
			{
			*theDecision = jnew JBooleanConstant(boolInfo[i-1].value);
			assert( *theDecision != NULL );
			return kJTrue;
			}
		}

	return kJFalse;
}

/******************************************************************************
 JParseAsNegation

	Syntax:  "not <decision>"

 ******************************************************************************/

JParseResult
JParseAsNegation
	(
	const JCharacter*		origExpr,
	const JSize				origLength,
	const JVariableList*	theVariableList,
	JDecision**				theDecision
	)
{
	*theDecision = NULL;

	// remove enclosing parentheses

	const JCharacter* expr = origExpr;
	const JSize length     = JStripParentheses(&expr, origLength);

	// must start with "not "

	if (!JStringBeginsWith(expr, length, JPGetBooleanNOTString()))
		{
		return kJNotMyProblem;
		}

	// parse the rest of the string as a decision

	const JSize offset = JPGetBooleanNOTStringLength();

	JDecision* arg = NULL;
	if (!JRecurseDecision(expr + offset, length - offset, theVariableList, &arg))
		{
		return kJParseError;
		}

	// return JBooleanNOT object

	*theDecision = jnew JBooleanNOT(arg);
	assert( *theDecision != NULL );
	return kJParsedOK;
}

/******************************************************************************
 JParseAsUnknown

	Checks if expr is a check for whether or not a variable value is known.

	Syntax:  "<variable name> is unknown"

 ******************************************************************************/

JParseResult
JParseAsUnknown
	(
	const JCharacter*		origExpr,
	const JSize				origLength,
	const JVariableList*	theVariableList,
	JDecision**				theDecision
	)
{
	*theDecision = NULL;

	// remove enclosing parentheses

	const JCharacter* expr = origExpr;
	const JSize length     = JStripParentheses(&expr, origLength);

	// search for " is unknown"

	if (!JStringEndsWith(expr, length, JPGetValueUnknownString()))
		{
		return kJNotMyProblem;
		}

	// check front part for a single variable name

	const JSize offset         = length - JPGetValueUnknownStringLength();
	const JCharacter* varStart = expr;
	const JSize varLength      = JStripParentheses(&varStart, offset);

	JIndex varIndex;
	JFunction* arrayIndex = NULL;

	const JParseResult result = JParseVariable(varStart, varLength, theVariableList,
											   &varIndex, &arrayIndex);
	if (result != kJParsedOK)
		{
		return result;
		}

	// return JValueUnknown object

	*theDecision = jnew JValueUnknown(theVariableList, varIndex, arrayIndex);
	assert( *theDecision != NULL );
	return kJParsedOK;
}

/******************************************************************************
 JParseAsDiscreteComparison

	Checks if expr is a comparison between a variable and a particular
	discrete value of that variable.  All discrete variables are included
	in this.  Comparing any variable to the appropriate equivalent of"unknown"
	is also included.

	Syntax:  "<variable name> = <constant value>"

	Also checks for comparison between two discrete variables that have
	identical sets of values.

	Syntax:  "<variable name> = <variable name>"

 ******************************************************************************/

JParseResult
JParseAsDiscreteComparison
	(
	const JCharacter*		origExpr,
	const JSize				origLength,
	const JVariableList*	theVariableList,
	JDecision**				theDecision
	)
{
	*theDecision = NULL;

	// remove enclosing parentheses

	const JCharacter* expr = origExpr;
	const JSize length     = JStripParentheses(&expr, origLength);

	// search for equal sign

	JSize offset;
	if (!JStringContains(expr, length, JPGetEqualityString(), &offset))
		{
		return kJNotMyProblem;
		}

	// check front part for a single variable name
	// if it doesn't work, we assume it's a function

	JIndex varIndex;
	JFunction* arrayIndex = NULL;

	const JParseResult result = JParseVariable(expr, offset, theVariableList,
											   &varIndex, &arrayIndex);
	if (result != kJParsedOK)
		{
		return result;
		}

	// check last part for a legal variable value

	const JSize valueOffset      = offset + JPGetEqualityStringLength();
	const JCharacter* valueStart = expr + valueOffset;
	const JSize valueLength      = JStripParentheses(&valueStart, length - valueOffset);

	JIndex value;
	if (theVariableList->IsNumeric(varIndex))
		{
		// check for comparison with appropriate equivalent of "unknown"

		const JString& unknownName = theVariableList->GetUnknownValueSymbol(varIndex);
		if (JStringsEqual(valueStart, valueLength, unknownName))
			{
			// return JValueUnknown object

			*theDecision = jnew JValueUnknown(theVariableList, varIndex, arrayIndex);
			assert( *theDecision != NULL );
			return kJParsedOK;
			}
		else
			{
			jdelete arrayIndex;
			return kJNotMyProblem;
			}
		}
	else if (theVariableList->ParseDiscreteValue(valueStart, valueLength, varIndex, &value))
		{
		// return JDiscreteEquality object

		*theDecision = jnew JDiscreteEquality(theVariableList, varIndex, arrayIndex, value);
		assert( *theDecision != NULL );
		return kJParsedOK;
		}

	// check for another discrete variable

	JIndex varIndex2;
	JFunction* arrayIndex2 = NULL;

	const JParseResult result2 = JParseVariable(valueStart, valueLength, theVariableList,
											    &varIndex2, &arrayIndex2);
	if (result2 == kJParsedOK && theVariableList->IsDiscrete(varIndex2) &&
		theVariableList->HaveSameValues(varIndex, varIndex2))
		{
		// return JDiscreteVarEquality object

		*theDecision = jnew JDiscreteVarEquality(theVariableList, varIndex, arrayIndex,
												varIndex2, arrayIndex2);
		assert( *theDecision != NULL );
		return kJParsedOK;
		}

	jdelete arrayIndex;
	jdelete arrayIndex2;

	if (result2 == kJParsedOK)
		{
		// it parsed, but the variables can't be compared

		JString errorStr = theVariableList->GetVariableName(varIndex);
		errorStr += " cannot be compared with ";
		errorStr += JString(valueStart, valueLength);
		(JGetUserNotification())->ReportError(errorStr);
		return kJParseError;
		}
	else if (result2 == kJParseError)
		{
		return kJParseError;
		}
	else
		{
		// we couldn't make sense of the second half, so it's an error

		JString errorStr(valueStart, valueLength);
		errorStr.Prepend("\"");
		errorStr += "\" is not a possible value of ";
		errorStr += theVariableList->GetVariableName(varIndex);
		(JGetUserNotification())->ReportError(errorStr);
		return kJParseError;
		}
}

/******************************************************************************
 JDiscreteValueNameValid

	Returns kJTrue if the string is a valid discrete value.  All characters
	except whitespace are legal.

 ******************************************************************************/

JBoolean
JDiscreteValueNameValid
	(
	const JCharacter* str
	)
{
JIndex i;

	const JSize length = strlen(str);
	if (length == 0)
		{
		(JGetUserNotification())->ReportError("Discrete values cannot be empty");
		return kJFalse;
		}

	for (i=1; i<=length; i++)
		{
		if (isspace(str[i-1]))
			{
			(JGetUserNotification())->ReportError("Discrete values cannot contain spaces");
			return kJFalse;
			}
		}

	const JSize opCount                 = JPGetComparisonOpCount();
	const JDecisionOperatorInfo* opInfo = JPGetComparisonOpInfo();

	for (i=0; i<opCount; i++)
		{
		JSize offset;
		if (JStringContains(str, length, opInfo[i].name, &offset))
			{
			(JGetUserNotification())->ReportError(
				"Discrete values cannot contain comparison operators");
			return kJFalse;
			}
		}

	return kJTrue;
}

/******************************************************************************
 JContainsBooleanOperator

	Returns the location and type of the lowest precedence Boolean operator
	that is closest to the end of the string.

 ******************************************************************************/

JBoolean
JContainsBooleanOperator
	(
	const JCharacter*	expr,
	const JSize			length,

	JDecisionType*		type,
	JSize*				start,
	JSize*				end
	)
{
	return JFindLowestPrecedenceOperator(expr, length, type, start, end,
										 JPGetBooleanOpCount(),
										 JPGetBooleanOpInfo());
}

/******************************************************************************
 JContainsComparisonOperator

	Returns the location and type of the lowest precedence Comparison operator
	that is closest to the end of the string.

 ******************************************************************************/

JBoolean
JContainsComparisonOperator
	(
	const JCharacter*	expr,
	const JSize			length,

	JDecisionType*		type,
	JSize*				start,
	JSize*				end
	)
{
	return JFindLowestPrecedenceOperator(expr, length, type, start, end,
										 JPGetComparisonOpCount(),
										 JPGetComparisonOpInfo());
}

/******************************************************************************
 JFindLowestPrecedenceOperator

	Returns the location and type of the lowest precedence operator
	that is closest to the end of the string.

	The op array should be ordered from lowest to highest precedence.

 ******************************************************************************/

#define JOperatorInfoType	JDecisionOperatorInfo
#define JOperatorType		JDecisionType
#include <jFindLPOperator.th>
