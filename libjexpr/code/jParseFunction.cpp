/******************************************************************************
 jParseFunction.cpp

							The JFunction Parser Module

	This module parses a string into a JFunction.

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#include <jParseFunction.h>
#include <jParseFunction.ph>
#include <jParseUtil.h>
#include <jParserData.h>

#include <JFunctionType.h>
#include <JSummation.h>
#include <JNegation.h>
#include <JProduct.h>
#include <JDivision.h>
#include <JExponent.h>
#include <JVariableValue.h>
#include <JConstantValue.h>
#include <JNamedConstant.h>

#include <JSquareRoot.h>
#include <JAbsValue.h>
#include <JAlgSign.h>
#include <JRoundToInt.h>
#include <JTruncateToInt.h>

#include <JLogE.h>

#include <JSine.h>
#include <JCosine.h>
#include <JTangent.h>
#include <JArcSine.h>
#include <JArcCosine.h>
#include <JArcTangent.h>

#include <JHypSine.h>
#include <JHypCosine.h>
#include <JHypTangent.h>
#include <JArcHypSine.h>
#include <JArcHypCosine.h>
#include <JArcHypTangent.h>

#include <JRealPart.h>
#include <JImagPart.h>
#include <JPhaseAngle.h>
#include <JConjugate.h>

#include <JLogB.h>
#include <JArcTangent2.h>
#include <JRotateComplex.h>

#include <JMaxFunc.h>
#include <JMinFunc.h>
#include <JParallel.h>

#include <JDiscVarValueIndex.h>
#include <JDiscVarValue.h>

#include <JUserInputFunction.h>

#include <JVariableList.h>

#include <ctype.h>
#include <jGlobals.h>
#include <jErrno.h>
#include <jAssert.h>

// Private declarations

JParseResult
JParseAsSummation(const JCharacter* origExpr, const JSize origLength,
				  const JVariableList* theVariableList, JFunction** theFunction,
				  const JBoolean allowUIF);
JParseResult
JParseAsProduct(const JCharacter* origExpr, const JSize origLength,
				const JVariableList* theVariableList, JFunction** theFunction,
				const JBoolean allowUIF);
JParseResult
JParseAsDivision(const JCharacter* origExpr, const JSize origLength,
				 const JVariableList* theVariableList, JFunction** theFunction,
				 const JBoolean allowUIF);
JParseResult
JParseAsExponentiation(const JCharacter* origExpr, const JSize origLength,
					   const JVariableList* theVariableList, JFunction** theFunction,
					   const JBoolean allowUIF);

JParseResult
JParseAsFunctionWithArgs(const JCharacter* origExpr, const JSize origLength,
						 const JVariableList* theVariableList, JFunction** theFunction,
						 const JBoolean allowUIF);
JParseResult
JParseAsFunctionOfDiscrete(const JCharacter* origExpr, const JSize origLength,
						   const JVariableList* theVariableList, JFunction** theFunction,
						   const JBoolean allowUIF);

JBoolean JIsNamedConstant(const JCharacter* origExpr, const JSize origLength,
						  JFunction** theFunction);
JParseResult JParseAsNumericVariable(const JCharacter* origExpr, const JSize origLength,
									 const JVariableList* theVariableList,
									 JFunction** theFunction, const JBoolean allowUIF);

int JIsExponentSign(const JCharacter* expr, const JSize opOffset, const JSize argOffset);

/******************************************************************************
 JParseFunction

	The main entry point for this module.  It initiates the recursive parsing
	of expr into a JFunction.

 ******************************************************************************/

JBoolean
JParseFunction
	(
	const JCharacter*		expr,
	const JVariableList*	theVariableList,
	JFunction**				theFunction,
	const JBoolean			allowUIF
	)
{
	return JRecurseFunction(expr, strlen(expr), theVariableList,
							theFunction, allowUIF);
}

/******************************************************************************
 JRecurseFunction

	Parse the indicated portion of the string into a JFunction.

 ******************************************************************************/

JBoolean
JRecurseFunction
	(
	const JCharacter*		origExpr,
	const JSize				origLength,
	const JVariableList*	theVariableList,
	JFunction**				theFunction,
	const JBoolean			allowUIF
	)
{
	*theFunction = NULL;

	// remove enclosing parentheses

	const JCharacter* expr = origExpr;
	const JSize length     = JStripParentheses(&expr, origLength);

	if (length == 0)
		{
		(JGetUserNotification())->ReportError("You specified an empty function");
		return kJFalse;
		}

	// check for summation (+,-)

	JParseResult result = JParseAsSummation(expr, length, theVariableList,
											theFunction, allowUIF);
	if (result == kJParsedOK)
		{
		return kJTrue;
		}
	else if (result == kJParseError)
		{
		return kJFalse;
		}

	// check for product before division
	// so we correctly parse x/y*z as (x/y)*z

	result = JParseAsProduct(expr, length, theVariableList, theFunction, allowUIF);
	if (result == kJParsedOK)
		{
		return kJTrue;
		}
	else if (result == kJParseError)
		{
		return kJFalse;
		}

	// check for division

	result = JParseAsDivision(expr, length, theVariableList, theFunction, allowUIF);
	if (result == kJParsedOK)
		{
		return kJTrue;
		}
	else if (result == kJParseError)
		{
		return kJFalse;
		}

	// check for exponentiation

	result = JParseAsExponentiation(expr, length, theVariableList, theFunction, allowUIF);
	if (result == kJParsedOK)
		{
		return kJTrue;
		}
	else if (result == kJParseError)
		{
		return kJFalse;
		}

	// check for standard function

	result = JParseAsFunctionWithArgs(expr, length, theVariableList, theFunction, allowUIF);
	if (result == kJParsedOK)
		{
		return kJTrue;
		}
	else if (result == kJParseError)
		{
		return kJFalse;
		}

	// check for function of discrete variable

	result = JParseAsFunctionOfDiscrete(expr, length, theVariableList, theFunction, allowUIF);
	if (result == kJParsedOK)
		{
		return kJTrue;
		}
	else if (result == kJParseError)
		{
		return kJFalse;
		}

	// check for named constant

	if (JIsNamedConstant(expr, length, theFunction))
		{
		return kJTrue;
		}

	// check for variable name

	result = JParseAsNumericVariable(expr, length, theVariableList, theFunction, allowUIF);
	if (result == kJParsedOK)
		{
		return kJTrue;
		}
	else if (result == kJParseError)
		{
		return kJFalse;
		}

	// check for empty JUserInputFunction

	if (allowUIF && JStringsEqual(expr, length, JUserInputFunction::GetEmptyString()))
		{
		*theFunction = jnew JUserInputFunction(theVariableList,
											  JGetCurrFontManager(),
											  JGetCurrColormap());
		assert( *theFunction != NULL );
		return kJTrue;
		}

	// check for constant, must occupy entire string

	JString valueStr(expr, length);
	JBoolean isPercentage = kJFalse;
	if (valueStr.GetLastCharacter() == '%')
		{
		isPercentage = kJTrue;
		valueStr.RemoveSubstring(valueStr.GetLength(), valueStr.GetLength());
		valueStr.TrimWhitespace();
		}

	JFloat value;
	if (valueStr.ConvertToFloat(&value))
		{
		if (isPercentage)
			{
			value /= 100.0;
			}

		*theFunction = jnew JConstantValue(value);
		assert( *theFunction != NULL );
		return kJTrue;
		}
	else if (jerrno() == ERANGE)
		{
		JString errorStr(expr, length);
		errorStr.Prepend("\"");
		errorStr += "\" is too large to be represented.";
		(JGetUserNotification())->ReportError(errorStr);
		return kJFalse;
		}

	// this string is not a function

	JString errorStr(expr, length);
	errorStr.Prepend("\"");
	errorStr += "\" is not a valid function.";
	(JGetUserNotification())->ReportError(errorStr);
	return kJFalse;
}

/******************************************************************************
 JParseAsSummation

	Checks if expr is a sum of expressions.  If so, parses each piece as a
	JFunction.  Subtraction is treated by inserting a unary minus.

	Syntax:  "(+-) <f1> + <f2> - <f3> + <f4> - ..."

 ******************************************************************************/

JParseResult
JParseAsSummation
	(
	const JCharacter*		origExpr,
	const JSize				origLength,
	const JVariableList*	theVariableList,
	JFunction**				theFunction,
	const JBoolean			allowUIF
	)
{
	*theFunction = NULL;

	// remove enclosing parentheses

	const JCharacter* expr = origExpr;
	const JSize length     = JStripParentheses(&expr, origLength);

	// build a list of the locations of all visible + and - operators

	const JCharacter* plusStr  = JPGetAdditionString();
	const JSize plusLength     = JPGetAdditionStringLength();
	const JCharacter* minusStr = JPGetSubtractionString();
	const JSize minusLength    = JPGetSubtractionStringLength();

	JArray<JCharacter> opList;
	JArray<JSize> opOffsetList;
	JArray<JSize> argOffsetList;
	JSize lastOffset = 0;
	while (1)
		{
		JSize plusOffset;
		const JBoolean foundPlus =
			JFindFirstOperator(expr + lastOffset, length - lastOffset,
							   plusStr, &plusOffset);
		plusOffset += lastOffset;

		JSize minusOffset;
		const JBoolean foundMinus =
			JFindFirstOperator(expr + lastOffset, length - lastOffset,
							   minusStr, &minusOffset);
		minusOffset += lastOffset;

		JCharacter opType;
		JSize opOffset, argOffset;
		if (foundPlus && foundMinus && plusOffset < minusOffset)
			{
			opType = '+';
			opOffset  = plusOffset;
			argOffset = plusOffset + plusLength;
			}
		else if (foundPlus && foundMinus && minusOffset < plusOffset)
			{
			opType = '-';
			opOffset  = minusOffset;
			argOffset = minusOffset + minusLength;
			}
		else if (foundPlus)
			{
			opType = '+';
			opOffset  = plusOffset;
			argOffset = plusOffset + plusLength;
			}
		else if (foundMinus)
			{
			opType = '-';
			opOffset  = minusOffset;
			argOffset = minusOffset + minusLength;
			}
		else
			{
			break;
			}

		if (!JIsExponentSign(expr, opOffset, argOffset))
			{
			opList.AppendElement(opType);
			opOffsetList.AppendElement(opOffset);
			argOffsetList.AppendElement(argOffset);
			}

		lastOffset = argOffset;
		}

	if (opList.IsEmpty())
		{
		return kJNotMyProblem;
		}
	else if (opList.GetElementCount() == 1 && opOffsetList.GetElement(1) == 0)
		{
		const JSize argOffset = argOffsetList.GetElement(1);
		JFunction* arg = NULL;
		if (!JRecurseFunction(expr + argOffset, length - argOffset, theVariableList,
							  &arg, allowUIF))
			{
			return kJParseError;
			}

		if (opList.GetElement(1) == '-')
			{
			*theFunction = jnew JNegation(arg);
			assert( *theFunction != NULL );
			return kJParsedOK;
			}
		else	// we ignore leading +
			{
			*theFunction = arg;
			return kJParsedOK;
			}
		}
	else
		{
		JSummation* sum = jnew JSummation;
		assert( sum != NULL );

		// No operand in front means implied +

		if (opOffsetList.GetElement(1) > 0)
			{
			opList.PrependElement('+');
			opOffsetList.PrependElement(0);
			argOffsetList.PrependElement(0);
			}

		// We append an extra element at the end of the list
		// so our loop catches the last summand.

		opList.AppendElement('+');
		opOffsetList.AppendElement(length);
		argOffsetList.AppendElement(length);

		// Parse each summand

		const JSize opCount = opList.GetElementCount();
		for (JIndex i=1; i<opCount; i++)
			{
			const JCharacter opType = opList.GetElement(i);
			const JSize startOffset = argOffsetList.GetElement(i);
			const JSize endOffset   = opOffsetList.GetElement(i+1);

			JFunction* arg = NULL;
			if (!JRecurseFunction(expr + startOffset, endOffset - startOffset,
								  theVariableList, &arg, allowUIF))
				{
				jdelete sum;
				return kJParseError;
				}
			else if (opType == '+')
				{
				sum->SetArg(i, arg);
				}
			else	// opType == '-'
				{
				JNegation* negArg = jnew JNegation(arg);
				assert( negArg != NULL );
				sum->SetArg(i, negArg);
				}
			}

		*theFunction = sum;
		return kJParsedOK;
		}
}

/******************************************************************************
 JParseAsProduct

	Checks if expr is a product of expressions.  If so, parses each piece as a
	JFunction.

	Syntax:  "<f1> * <f2> * <f3> * <f4> * ..."

 ******************************************************************************/

JParseResult
JParseAsProduct
	(
	const JCharacter*		origExpr,
	const JSize				origLength,
	const JVariableList*	theVariableList,
	JFunction**				theFunction,
	const JBoolean			allowUIF
	)
{
	*theFunction = NULL;

	// remove enclosing parentheses

	const JCharacter* expr = origExpr;
	const JSize length     = JStripParentheses(&expr, origLength);

	// parse expressions between * operators

	const JCharacter* multStr = JPGetMultiplicationString();
	const JSize opLength      = JPGetMultiplicationStringLength();

	JProduct* prod = NULL;
	JSize argCount = 0;

	JSize lastOffset = 0;
	while (1)
		{
		JSize offset;
		const JBoolean found =
			JFindFirstOperator(expr + lastOffset, length - lastOffset,
							   multStr, &offset);

		if (!found && prod == NULL)
			{
			return kJNotMyProblem;
			}
		else if (!found)
			{
			JFunction* arg = NULL;
			if (JRecurseFunction(expr + lastOffset, length - lastOffset,
								 theVariableList, &arg, allowUIF))
				{
				argCount++;
				prod->SetArg(argCount, arg);
				*theFunction = prod;
				return kJParsedOK;
				}
			else
				{
				jdelete prod;
				return kJParseError;
				}
			}
		else
			{
			if (prod == NULL)
				{
				prod = jnew JProduct;
				assert( prod != NULL );
				}

			JFunction* arg = NULL;
			if (JRecurseFunction(expr + lastOffset, offset, theVariableList,
								 &arg, allowUIF))
				{
				argCount++;
				prod->SetArg(argCount, arg);
				}
			else
				{
				jdelete prod;
				return kJParseError;
				}
			}

		lastOffset += offset + opLength;
		}
}

/******************************************************************************
 JParseAsDivision

	Checks if expr is a quotient of expressions.  If so, parses each piece as a
	JFunction.

	Syntax:  "<f1> / <f2>"

 ******************************************************************************/

JParseResult
JParseAsDivision
	(
	const JCharacter*		origExpr,
	const JSize				origLength,
	const JVariableList*	theVariableList,
	JFunction**				theFunction,
	const JBoolean			allowUIF
	)
{
	*theFunction = NULL;

	// remove enclosing parentheses

	const JCharacter* expr = origExpr;
	const JSize length     = JStripParentheses(&expr, origLength);

	// find the last division operator

	JSize offset;
	const JBoolean found = JFindLastOperator(expr, length, JPGetDivisionString(), &offset);
	if (!found)
		{
		return kJNotMyProblem;
		}

	JFunction* numerator   = NULL;
	JFunction* denominator = NULL;

	const JSize offset2 = offset + JPGetDivisionStringLength();

	if (JRecurseFunction(expr, offset, theVariableList, &numerator, allowUIF) &&
		JRecurseFunction(expr + offset2, length - offset2, theVariableList,
						 &denominator, allowUIF))
		{
		JDivision* div = jnew JDivision(numerator, denominator);
		assert( div != NULL );
		*theFunction = div;
		return kJParsedOK;
		}
	else
		{
		jdelete numerator;
		jdelete denominator;
		return kJParseError;
		}
}

/******************************************************************************
 JParseAsExponentiation

	Checks if expr is an exponentiation.  If so, parses each piece as a
	JFunction.

	Syntax:  "<f1> ^ <f2>"

 ******************************************************************************/

JParseResult
JParseAsExponentiation
	(
	const JCharacter*		origExpr,
	const JSize				origLength,
	const JVariableList*	theVariableList,
	JFunction**				theFunction,
	const JBoolean			allowUIF
	)
{
	*theFunction = NULL;

	// remove enclosing parentheses

	const JCharacter* expr = origExpr;
	const JSize length     = JStripParentheses(&expr, origLength);

	// find the first exponent operator

	JSize offset;
	const JBoolean found = JFindFirstOperator(expr, length, JPGetExponentString(), &offset);
	if (!found)
		{
		return kJNotMyProblem;
		}

	JFunction* arg1 = NULL;
	JFunction* arg2 = NULL;

	const JSize offset2 = offset + JPGetExponentStringLength();

	if (JRecurseFunction(expr, offset, theVariableList, &arg1, allowUIF) &&
		JRecurseFunction(expr + offset2, length - offset2, theVariableList,
						 &arg2, allowUIF))
		{
		JExponent* exponent = jnew JExponent(arg1, arg2);
		assert( exponent != NULL );
		*theFunction = exponent;
		return kJParsedOK;
		}
	else
		{
		jdelete arg1;
		jdelete arg2;
		return kJParseError;
		}
}

/******************************************************************************
 JParseAsFunctionWithArgs

	Checks if expr begins with a function name like sin(, max(, etc.
	If so, parses the rest of the string as the arguments, separated by commas.

	Syntax:  "<function name>(<arg1>,<arg2>,...)"

 ******************************************************************************/

JParseResult
JParseAsFunctionWithArgs
	(
	const JCharacter*		origExpr,
	const JSize				origLength,
	const JVariableList*	theVariableList,
	JFunction**				theFunction,
	const JBoolean			allowUIF
	)
{
	*theFunction = NULL;

	// remove enclosing parentheses

	const JCharacter* expr = origExpr;
	const JSize length     = JStripParentheses(&expr, origLength);

	// check if expr starts with a function name

	const JSize fnCount            = JPGetStdFunctionCount();
	const JStdFunctionInfo* fnInfo = JPGetStdFunctionInfo();

	JBoolean found = kJFalse;
	JFunctionType type = kJSquareRootType;
	const JCharacter* argsStart = NULL;
	JSize argsLength = 0, argCount = 0, nameLength = 0;
	for (JIndex i=1; i<=fnCount; i++)
		{
		if (JStringBeginsWith(expr, length, fnInfo[i-1].name))
			{
			found    = kJTrue;
			type     = fnInfo[i-1].type;
			argCount = fnInfo[i-1].argCount;

			nameLength = strlen(fnInfo[i-1].name);
			argsStart  = expr + nameLength;
			argsLength = length - nameLength - 1;

			break;
			}
		}
	if (!found)
		{
		return kJNotMyProblem;
		}

	// check if the argument block is correctly closed

	if (!((*(expr + nameLength-1) == '(' && *(expr + length-1) == ')') ||
		  (*(expr + nameLength-1) == '[' && *(expr + length-1) == ']')) )
		{
		return kJNotMyProblem;
		}

	// create the appropriate object

	JFunctionWithArgs* stdFunction = NULL;
	if (type == kJSquareRootType)
		{
		stdFunction = jnew JSquareRoot();
		}
	else if (type == kJAbsValueType)
		{
		stdFunction = jnew JAbsValue();
		}
	else if (type == kJSignType)
		{
		stdFunction = jnew JAlgSign();
		}
	else if (type == kJRoundType)
		{
		stdFunction = jnew JRoundToInt();
		}
	else if (type == kJTruncateType)
		{
		stdFunction = jnew JTruncateToInt();
		}

	else if (type == kJLog10Type)
		{
		stdFunction = jnew JLogB();	// will set base later
		}
	else if (type == kJLogEType)
		{
		stdFunction = jnew JLogE();
		}
	else if (type == kJLog2Type)
		{
		stdFunction = jnew JLogB();	// will set base later
		}

	else if (type == kJSineType)
		{
		stdFunction = jnew JSine();
		}
	else if (type == kJCosineType)
		{
		stdFunction = jnew JCosine();
		}
	else if (type == kJTangentType)
		{
		stdFunction = jnew JTangent();
		}
	else if (type == kJArcSineType)
		{
		stdFunction = jnew JArcSine();
		}
	else if (type == kJArcCosineType)
		{
		stdFunction = jnew JArcCosine();
		}
	else if (type == kJArcTangentType)
		{
		stdFunction = jnew JArcTangent();
		}

	else if (type == kJHypSineType)
		{
		stdFunction = jnew JHypSine();
		}
	else if (type == kJHypCosineType)
		{
		stdFunction = jnew JHypCosine();
		}
	else if (type == kJHypTangentType)
		{
		stdFunction = jnew JHypTangent();
		}
	else if (type == kJArcHypSineType)
		{
		stdFunction = jnew JArcHypSine();
		}
	else if (type == kJArcHypCosineType)
		{
		stdFunction = jnew JArcHypCosine();
		}
	else if (type == kJArcHypTangentType)
		{
		stdFunction = jnew JArcHypTangent();
		}

	else if (type == kJRealPartType)
		{
		stdFunction = jnew JRealPart();
		}
	else if (type == kJImagPartType)
		{
		stdFunction = jnew JImagPart();
		}
	else if (type == kJPhaseAngleType)
		{
		stdFunction = jnew JPhaseAngle();
		}
	else if (type == kJConjugateType)
		{
		stdFunction = jnew JConjugate();
		}

	else if (type == kJLogBType)
		{
		stdFunction = jnew JLogB();
		}
	else if (type == kJArcTangent2Type)
		{
		stdFunction = jnew JArcTangent2();
		}
	else if (type == kJRotateType)
		{
		stdFunction = jnew JRotateComplex();
		}

	else if (type == kJMaxFuncType)
		{
		stdFunction = jnew JMaxFunc();
		}
	else if (type == kJMinFuncType)
		{
		stdFunction = jnew JMinFunc();
		}
	else if (type == kJParallelType)
		{
		stdFunction = jnew JParallel();
		}
	else
		{
		std::cerr << "JParseAsFunctionWithArgs:  unknown function type" << std::endl;
		return kJParseError;
		}
	assert( stdFunction != NULL );

	// parse each argument

	const JCharacter* argSeparatorStr = JPGetArgSeparatorString();
	const JSize argSeparatorLength    = JPGetArgSeparatorStringLength();

	if (argCount == 1)
		{
		// parse the entire string as the single argument

		JFunction* arg = NULL;
		const JBoolean ok =
			JRecurseFunction(argsStart, argsLength, theVariableList, &arg, allowUIF);
		if (ok && (type == kJLog10Type || type == kJLog2Type))
			{
			JConstantValue* base = NULL;
			if (type == kJLog10Type)
				{
				base = jnew JConstantValue(10.0);
				}
			else if (type == kJLog2Type)
				{
				base = jnew JConstantValue(2.0);
				}
			assert( base != NULL );
			stdFunction->SetArg(1, base);
			stdFunction->SetArg(2, arg);
			*theFunction = stdFunction;
			return kJParsedOK;
			}
		else if (ok)
			{
			stdFunction->SetArg(1, arg);
			*theFunction = stdFunction;
			return kJParsedOK;
			}
		else
			{
			jdelete stdFunction;
			return kJParseError;
			}
		}
	else if (argCount != kJUnlimitedArgs)
		{
		// parse all but the last argument by searching forward
		// for commas with JFindFirstOperator

		const JCharacter* argStart = argsStart;
		JSize remainderLength = argsLength;
		for (JIndex i=1; i<argCount; i++)
			{
			JSize offset;
			if (!JFindFirstOperator(argStart, remainderLength, argSeparatorStr, &offset))
				{
				JString errorStr(expr, length);
				errorStr.Prepend("\"");
				errorStr += "\" has too few arguments";
				(JGetUserNotification())->ReportError(errorStr);
				jdelete stdFunction;
				return kJParseError;
				}
			JFunction* arg = NULL;
			if (!JRecurseFunction(argStart, offset, theVariableList, &arg, allowUIF))
				{
				jdelete stdFunction;
				return kJParseError;
				}
			stdFunction->SetArg(i, arg);
			argStart += offset + argSeparatorLength;
			remainderLength -= offset + argSeparatorLength;
			}

		// make sure that there is only one argument left

		JSize offset;
		if (JFindFirstOperator(argStart, remainderLength, argSeparatorStr, &offset))
			{
			JString errorStr(expr, length);
			errorStr.Prepend("\"");
			errorStr += "\" has too many arguments";
			(JGetUserNotification())->ReportError(errorStr);
			jdelete stdFunction;
			return kJParseError;
			}

		// parse the last argument in the list

		JFunction* arg = NULL;
		if (!JRecurseFunction(argStart, remainderLength, theVariableList, &arg, allowUIF))
			{
			jdelete stdFunction;
			return kJParseError;
			}
		stdFunction->SetArg(argCount, arg);
		*theFunction = stdFunction;
		return kJParsedOK;
		}
	else	// argCount == kJUnlimitedArgs
		{
		// parse all the arguments by searching forward
		// for commas with JFindFirstOperator

		JIndex i=1;
		const JCharacter* argStart = argsStart;
		JSize remainderLength = argsLength;
		JBoolean foundArgSeparator = kJTrue;
		while (foundArgSeparator)
			{
			JSize offset;
			foundArgSeparator =
				JFindFirstOperator(argStart, remainderLength, argSeparatorStr, &offset);
			if (!foundArgSeparator)
				{
				offset = remainderLength;
				}
			JFunction* arg = NULL;
			if (!JRecurseFunction(argStart, offset, theVariableList, &arg, allowUIF))
				{
				jdelete stdFunction;
				return kJParseError;
				}
			stdFunction->SetArg(i, arg);
			i++;
			argStart += offset + argSeparatorLength;
			remainderLength -= offset + argSeparatorLength;
			}

		*theFunction = stdFunction;
		return kJParsedOK;
		}
}

/******************************************************************************
 JParseAsFunctionOfDiscrete

	Checks if expr begins with the name of a function of a discrete variable.
	If so, parses the rest of the string as the variable name.

	Syntax:  "<function name>(<discrete variable>)"

 ******************************************************************************/

JParseResult
JParseAsFunctionOfDiscrete
	(
	const JCharacter*		origExpr,
	const JSize				origLength,
	const JVariableList*	theVariableList,
	JFunction**				theFunction,
	const JBoolean			allowUIF
	)
{
	*theFunction = NULL;

	// remove enclosing parentheses

	const JCharacter* expr = origExpr;
	const JSize length     = JStripParentheses(&expr, origLength);

	// check if expr starts with a function name

	const JSize fnCount                   = JPGetFnOfDiscreteCount();
	const JFunctionOfDiscreteInfo* fnInfo = JPGetFnOfDiscreteInfo();

	JBoolean found = kJFalse;
	JFunctionType type = kJSquareRootType;
	const JCharacter* argStart = NULL;
	JSize argLength = 0, nameLength = 0;
	for (JSize i=1; i<=fnCount; i++)
		{
		if (JStringBeginsWith(expr, length, fnInfo[i-1].name))
			{
			found = kJTrue;
			type  = fnInfo[i-1].type;

			nameLength = strlen(fnInfo[i-1].name);
			argStart  = expr + nameLength;
			argLength = length - nameLength - 1;
			}
		}
	if (!found)
		{
		return kJNotMyProblem;
		}

	// check if the argument block is correctly closed

	if (!((*(expr + nameLength-1) == '(' && *(expr + length-1) == ')') ||
		  (*(expr + nameLength-1) == '[' && *(expr + length-1) == ']')) )
		{
		return kJNotMyProblem;
		}

	// parse the argument as a discrete variable name

	JIndex varIndex;
	JFunction* arrayIndex = NULL;

	const JParseResult result = JParseVariable(argStart, argLength, theVariableList,
											   &varIndex, &arrayIndex, allowUIF);
	if (result != kJParsedOK || !theVariableList->IsDiscrete(varIndex))
		{
		jdelete arrayIndex;

		JString errorStr(argStart, argLength);
		errorStr.Prepend("\"");
		errorStr += "\" is not a discrete variable";
		(JGetUserNotification())->ReportError(errorStr);
		return kJParseError;
		}

	// create the appropriate object

	if (type == kJDiscVarValueIndexType)
		{
		*theFunction = jnew JDiscVarValueIndex(theVariableList, varIndex, arrayIndex);
		}
	else if (type == kJDiscVarValueType)
		{
		*theFunction = jnew JDiscVarValue(theVariableList, varIndex, arrayIndex);
		}
	else
		{
		std::cerr << "JParseAsFunctionOfDiscrete:  unknown function type" << std::endl;
		jdelete arrayIndex;
		return kJParseError;
		}
	assert( *theFunction != NULL );

	return kJParsedOK;
}

/******************************************************************************
 JIsNamedConstant

	Checks if expr is a named constant like e, pi, etc.

 ******************************************************************************/

JBoolean
JIsNamedConstant
	(
	const JCharacter*	origExpr,
	const JSize			origLength,
	JFunction**			theFunction
	)
{
	*theFunction = NULL;

	// remove enclosing parentheses

	const JCharacter* expr = origExpr;
	const JSize length     = JStripParentheses(&expr, origLength);

	// check if expr is a named constant

	const JSize nameCount           = JPGetNamedConstCount();
	const JNamedConstInfo* nameInfo = JPGetNamedConstInfo();

	for (JSize i=1; i<=nameCount; i++)
		{
		if (JStringsEqual(expr, length, nameInfo[i-1].name))
			{
			*theFunction = jnew JNamedConstant(nameInfo[i-1].index);
			assert( *theFunction != NULL );
			return kJTrue;
			}
		}
	return kJFalse;
}

/******************************************************************************
 JParseAsNumericVariable

	Checks if expr is a numeric variable in our variable list.

	Syntax:  "<variable name>[<function>]"

 ******************************************************************************/

JParseResult
JParseAsNumericVariable
	(
	const JCharacter*		origExpr,
	const JSize				origLength,
	const JVariableList*	theVariableList,
	JFunction**				theFunction,
	const JBoolean			allowUIF
	)
{
	*theFunction = NULL;

	// remove enclosing parentheses

	const JCharacter* expr = origExpr;
	const JSize length     = JStripParentheses(&expr, origLength);

	// try to parse the expression as a single variable

	JIndex varIndex;
	JFunction* arrayIndex = NULL;

	const JParseResult result = JParseVariable(expr, length, theVariableList,
											   &varIndex, &arrayIndex, allowUIF);

	if (result == kJParsedOK && !theVariableList->IsNumeric(varIndex))
		{
		jdelete arrayIndex;

		JString errorStr(expr, length);
		errorStr.Prepend("\"");
		errorStr += "\" is not a numeric variable";
		(JGetUserNotification())->ReportError(errorStr);
		return kJParseError;
		}
	else if (result == kJParsedOK)
		{
		*theFunction = jnew JVariableValue(theVariableList, varIndex, arrayIndex);
		assert( *theFunction != NULL );
		return kJParsedOK;
		}
	else
		{
		return result;
		}
}

/******************************************************************************
 JParseVariable

	Tries to parse expr as a variable.

	Syntax:  "<variable name>[<function>]"

 ******************************************************************************/

JParseResult
JParseVariable
	(
	const JCharacter*		origExpr,
	const JSize				origLength,
	const JVariableList*	theVariableList,
	JIndex*					varIndex,
	JFunction**				arrayIndex,
	const JBoolean			allowUIF
	)
{
	*arrayIndex = NULL;

	// remove enclosing parentheses

	const JCharacter* expr = origExpr;
	JSize length           = JStripParentheses(&expr, origLength);

	// strip off array index first

	JSize offset;
	if (JStringContains(expr, length, "[", &offset) &&
		JStringEndsWith(expr, length, "]"))
		{
		const JCharacter* indexExpr = expr + offset + 1;	// move past [
		JSize indexLength = length - offset - 2;			// ignore trailing ]
		if (!JRecurseFunction(indexExpr, indexLength, theVariableList, arrayIndex, allowUIF))
			{
			return kJParseError;
			}
		length = offset;
		}

	// look for variable name

	if (theVariableList->ParseVariableName(expr, length, varIndex))
		{
		const JBoolean isArray = theVariableList->IsArray(*varIndex);
		if (*arrayIndex == NULL && isArray)
			{
			JString errorStr(expr, length);
			errorStr.Prepend("\"");
			errorStr += "\" is an array";
			(JGetUserNotification())->ReportError(errorStr);
			return kJParseError;
			}
		if (*arrayIndex != NULL && !isArray)
			{
			jdelete *arrayIndex;
			*arrayIndex = NULL;

			JString errorStr(expr, length);
			errorStr.Prepend("\"");
			errorStr += "\" is not an array";
			(JGetUserNotification())->ReportError(errorStr);
			return kJParseError;
			}
		return kJParsedOK;
		}
	else if (*arrayIndex != NULL)
		{
		jdelete *arrayIndex;
		*arrayIndex = NULL;

		JString errorStr(expr, length);
		errorStr.Prepend("\"");
		errorStr += "\" is not a variable";
		(JGetUserNotification())->ReportError(errorStr);
		return kJParseError;
		}
	else
		{
		return kJNotMyProblem;
		}
}

/******************************************************************************
 JNameValid

	Returns kJTrue if the string is a valid name for a variable or a constant.
	Valid characters are the alphabet, digits, underscore (_), question mark (?),
	pound sign (#), dollar sign ($), percent sign (%), and sign (&), at sign (@),
	colon (:), period (.), and single back-quote (`).

	The first character must be a letter or a single back-quote (`).
	The single back-quote is used to denote a greek character.
	As long as names can't start with a digit, JIsExponentSign is safe.
	JUserInputFunctions are parsed from a single question mark, so we can't
	start with '?' either, because then '?' would be a valid variable name.

 ******************************************************************************/

static JCharacter validChar[] = {'_', '?', '#', '$', '%', '&', '@', ':', '.', '`'};
static JSize validCharCount   = sizeof(validChar)/sizeof(JCharacter);

JBoolean
JNameValid
	(
	const JCharacter* str
	)
{
JIndex i;

	const JSize length = strlen(str);
	if (length == 0)
		{
		(JGetUserNotification())->ReportError("Names cannot be empty.");
		return kJFalse;
		}

	const JCharacter greekPrefix = JPGetGreekCharPrefixChar();

	for (i=1; i<=length; i++)
		{
		const JCharacter c = str[i-1];

		JBoolean valid = JConvertToBoolean( (i>1 && JIsAlnum(c)) || (i==1 && JIsAlpha(c)) );
		if (i == 1)
			{
			valid = JConvertToBoolean( valid || c == greekPrefix );
			}
		else if (i > 1)
			{
			JSize j = 0;
			while (!valid && j < validCharCount)
				{
				valid = JConvertToBoolean( c == validChar[j] );
				j++;
				}
			}

		if (!valid)
			{
			JString errorStr = "Illegal character 'x";
			errorStr.SetCharacter(errorStr.GetLength(), c);
			errorStr += "' in \"";
			errorStr += str;
			errorStr += "\".";
			(JGetUserNotification())->ReportError(errorStr);
			return kJFalse;
			}
		}

	const JSize nameCount           = JPGetNamedConstCount();
	const JNamedConstInfo* nameInfo = JPGetNamedConstInfo();

	for (i=1; i<=nameCount; i++)
		{
		if (strcmp(str, nameInfo[i-1].name) == 0)
			{
			JString errorStr = "\"";
			errorStr += nameInfo[i-1].name;
			errorStr += "\" is already used to represent a constant.";
			(JGetUserNotification())->ReportError(errorStr);
			return kJFalse;
			}
		}

	return kJTrue;
}

/******************************************************************************
 JIsExponentSign

	Checks if the operator (+ or -) starting at location opOffset and
	ending at location argOffset-1 is the sign of an exponent.

	This function assumes that variable names can't start with a digit.

	Syntax:  <digit>(.)<E,e><+,-><digit>

 ******************************************************************************/

int
JIsExponentSign
	(
	const JCharacter*	expr,
	const JSize			opOffset,
	const JSize			argOffset
	)
{
	return
		(
		opOffset>=2 && isdigit(*(expr + argOffset)) &&
		tolower(*(expr + opOffset-1)) == 'e' &&
		(
		(isdigit(*(expr + opOffset-2))) ||
		(opOffset>=3 && isdigit(*(expr + opOffset-3)) && *(expr + opOffset-2) == '.')
		)
		);
}
