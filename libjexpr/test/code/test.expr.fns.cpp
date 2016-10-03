/******************************************************************************
 test.expr.fns.cpp

	These routines help test the parsing system.

	Copyright (C) 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "test.expr.fns.h"
#include "TestVarList.h"

#include <jParseDecision.h>
#include <jParseFunction.h>
#include <JDecision.h>
#include <JFunction.h>

#include <JString.h>
#include <jFStreamUtil.h>
#include <jStreamUtil.h>
#include <jGlobals.h>
#include <jAssert.h>

/******************************************************************************
 TestDecisionEquality

 ******************************************************************************/

void
TestDecisionEquality
	(
	const JCharacter* fileName
	)
{
	ifstream input(fileName);

	TestVarList theVarList(input);

	JDecision* d1 = NULL;
	JDecision* d2 = NULL;
	while (1)
		{
		if (!GetDecision(input, &theVarList, &d1))
			{
			break;
			}
		else if (d1 == NULL)
			{
			continue;
			}
		d1->Print(cout);
		cout << endl;

		if (!GetDecision(input, &theVarList, &d2))
			{
			delete d1;
			break;
			}
		else if (d2 == NULL)
			{
			delete d1;
			continue;
			}
		d2->Print(cout);
		cout << endl;

		if (*d1 == *d2)
			{
			(JGetUserNotification())->DisplayMessage("These decisions are the same");
			}
		else
			{
			(JGetUserNotification())->DisplayMessage("These decisions are not the same");
			}

		delete d1;
		delete d2;
		}
}

/******************************************************************************
 TestFunctionEquality

 ******************************************************************************/

void
TestFunctionEquality
	(
	const JCharacter* fileName
	)
{
	ifstream input(fileName);

	TestVarList theVarList(input);

	JFunction* f1 = NULL;
	JFunction* f2 = NULL;
	while (1)
		{
		if (!GetFunction(input, &theVarList, &f1))
			{
			break;
			}
		else if (f1 == NULL)
			{
			continue;
			}

		if (!GetFunction(input, &theVarList, &f2))
			{
			delete f1;
			break;
			}
		else if (f2 == NULL)
			{
			delete f1;
			continue;
			}

		if (*f1 == *f2)
			{
			(JGetUserNotification())->DisplayMessage("These functions are the same");
			}
		else
			{
			(JGetUserNotification())->DisplayMessage("These functions are not the same");
			}

		delete f1;
		delete f2;
		}
}

/******************************************************************************
 GetDecision

 ******************************************************************************/

JBoolean
GetDecision
	(
	istream&				input,
	const JVariableList*	theVariableList,
	JDecision**				theDecision
	)
{
	input >> ws;
	if (input.eof() || input.fail())
		{
		return kJFalse;
		}

	const JString expr = JReadUntil(input, '\n');
	cout << expr << endl;
	JParseDecision(expr, theVariableList, theDecision);
	return kJTrue;
}

/******************************************************************************
 GetFunction

 ******************************************************************************/

JBoolean
GetFunction
	(
	istream&				input,
	const JVariableList*	theVariableList,
	JFunction**				theFunction
	)
{
	input >> ws;
	if (input.eof() || input.fail())
		{
		return kJFalse;
		}

	const JString expr = JReadUntil(input, '\n');
	cout << expr << endl;
	JParseFunction(expr, theVariableList, theFunction);
	return kJTrue;
}
