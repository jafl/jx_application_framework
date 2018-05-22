/******************************************************************************
 jExprUIUtil.cpp

	Useful functions for expression editing system.

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include <jExprUIUtil.h>

#include <jParseFunction.h>
#include <jParserData.h>
#include <JVariableList.h>
#include <JFunctionWithArgs.h>
#include <JUserInputFunction.h>

#include <JString.h>
#include <jGlobals.h>
#include <jAssert.h>

/******************************************************************************
 JApplyFunction

	Apply the specified function to the given JFunction.  Returns the
	resulting function, the copy of the argument that was passed in,
	and the first new UIF, if successful.

 ******************************************************************************/

JBoolean
JApplyFunction
	(
	const JCharacter*		fnName,
	const JVariableList*	varList,
	const JFunction&		origF,
	const JFontManager*		fontMgr,
	JXColorManager*				colormap,

	JFunction**				newF,
	JFunction**				newArg,
	JUserInputFunction**	newUIF
	)
{
	*newF   = NULL;
	*newUIF = NULL;

	JString buffer = fnName;
	buffer.TrimWhitespace();
	if (buffer.GetLastCharacter() != '(')
		{
		buffer.Append("(");
		}

	const JSize fnCount            = JPGetStdFunctionCount();
	const JStdFunctionInfo* fnInfo = JPGetStdFunctionInfo();

	JBoolean found = kJFalse;
	JSize argCount = 0;
	for (JIndex i=1; i<=fnCount; i++)
		{
		if (buffer == fnInfo[i-1].name)
			{
			found    = kJTrue;
			argCount = fnInfo[i-1].argCount;
			break;
			}
		}

	if (found)
		{
		for (JIndex i=1; i<argCount; i++)
			{
			buffer += "1";
			buffer += JPGetArgSeparatorString();
			}
		buffer += "1)";
		}
	else
		{
		JIndex parenIndex;
		const JBoolean found = buffer.LocateSubstring("(", &parenIndex);
		assert( found );
		buffer.RemoveSubstring(parenIndex, buffer.GetLength());
		JString errorStr = "Unknown function \"";
		errorStr += buffer;
		errorStr += "\"";
		(JGetUserNotification())->ReportError(errorStr);
		return kJFalse;
		}

	const JSize origArgCount = argCount;

	const JBoolean ok = JParseFunction(buffer, varList, newF);
	if (ok)
		{
		*newArg = origF.Copy();

		const JFunctionType newType = (**newF).GetType();
		JFunctionWithArgs* fwa = (**newF).CastToJFunctionWithArgs();
		assert( fwa != NULL );
		argCount = fwa->GetArgCount();
		for (JIndex i=1; i<=argCount; i++)
			{
			if (i == 2 && newType == kJLogBType)
				{
				fwa->SetArg(2, *newArg);
				}
			else if (i == 1 && newType != kJLogBType)
				{
				fwa->SetArg(1, *newArg);
				}
			else if (newType != kJLogBType || origArgCount > 1)
				{
				JUserInputFunction* uif =
					jnew JUserInputFunction(varList, fontMgr, colormap);
				assert( uif != NULL );
				fwa->SetArg(i, uif);
				if (*newUIF == NULL)
					{
					*newUIF = uif;
					}
				}
			}
		}
	return ok;
}
