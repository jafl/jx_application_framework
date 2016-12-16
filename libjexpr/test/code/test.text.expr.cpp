/******************************************************************************
 test.text.expr.cpp

	This provides a way to test the parsing system and JTextExprRenderer.

	Copyright (C) 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "test.parser.fns.h"
#include "TestVarList.h"

#include <jParseDecision.h>
#include <jParseFunction.h>
#include <JDecision.h>
#include <JFunction.h>

#include <JString.h>
#include <JColorIndex.h>
#include <jCommandLine.h>
#include <jFStreamUtil.h>
#include <jStreamUtil.h>
#include <stdio.h>
#include <jGlobals.h>
#include <jAssert.h>

// Private routines

void TestDecisionParser(const JCharacter* fileName);
void TestFunctionParser(const JCharacter* fileName);
void TestComplexFunctionParser(const JCharacter* fileName);

/******************************************************************************
 main

 ******************************************************************************/

int
main
	(
	int		argc,
	char*	argv[]
	)
{
	// create global objects

	JInitCore();

	// ask the user what to do

	JBoolean done = kJFalse;
	while (!done)
		{
		std::cout << std::endl << std::endl;
		std::cout << " 1) Test Function parser" << std::endl;
		std::cout << " 2) Test Decision parser" << std::endl;
		std::cout << " 3) Test Function equality" << std::endl;
		std::cout << " 4) Test Decision equality" << std::endl;
		std::cout << " 5) Test Complex Function parser" << std::endl;
		std::cout << " 0) Done" << std::endl;
		std::cout << std::endl;
		std::cout << "Enter choice: ";

		long action;
		std::cin >> action;
		JInputFinished();

		if (1 <= action && action <= 5 &&
			!(JGetChooseSaveFile())->ChooseFile("Input file:"))
			{
			continue;
			}
		const JString fileName = (JGetChooseSaveFile())->GetChosenPathAndName();

		switch (action)
			{
			case 1:
				TestFunctionParser(fileName);
				break;

			case 2:
				TestDecisionParser(fileName);
				break;

			case 3:
				TestFunctionEquality(fileName);
				break;

			case 4:
				TestDecisionEquality(fileName);
				break;

			case 5:
				TestComplexFunctionParser(fileName);
				break;

			case 0:
				done = kJTrue;
				break;

			default:
				(JGetUserNotification())->ReportError("That feature is not yet available");
				break;
			}
		}

	// clean up

	return 0;
}

/******************************************************************************
 TestDecisionParser

 ******************************************************************************/

void
TestDecisionParser
	(
	const JCharacter* fileName
	)
{
	std::ifstream input(fileName);

	TestVarList theVarList(input);

	while (input.good())
		{
		JBoolean expectedResult;
		input >> expectedResult;

		JDecision* theDecision = NULL;
		if (!GetDecision(input, &theVarList, &theDecision))
			{
			break;
			}
		else if (theDecision != NULL)
			{
			theDecision->Print(std::cout);
			std::cout << std::endl;
			std::cout << "Expected: " << expectedResult << std::endl;
			std::cout << "Result:   " << theDecision->Evaluate() << std::endl;
			JWaitForReturn();
			jdelete theDecision;
			}

		input >> std::ws;
		}
}

/******************************************************************************
 TestFunctionParser

 ******************************************************************************/

void
TestFunctionParser
	(
	const JCharacter* fileName
	)
{
	std::ifstream input(fileName);

	TestVarList theVarList(input);

	while (input.good())
		{
		JBoolean expectedOK;
		input >> expectedOK;

		JFloat expectedResult;
		input >> expectedResult;

		JFunction* theFunction = NULL;
		if (!GetFunction(input, &theVarList, &theFunction))
			{
			break;
			}
		else if (theFunction != NULL)
			{
			theFunction->Print(std::cout);
			std::cout << std::endl;

			JFloat result;
			const JBoolean ok = theFunction->Evaluate(&result);
			std::cout << "Expected: " << expectedOK << ' ' << expectedResult << std::endl;
			std::cout << "Result:   " << ok << ' ' << result << std::endl;
			JWaitForReturn();

			jdelete theFunction;
			}

		input >> std::ws;
		}
}

/******************************************************************************
 TestComplexFunctionParser

 ******************************************************************************/

void
TestComplexFunctionParser
	(
	const JCharacter* fileName
	)
{
	std::ifstream input(fileName);

	TestVarList theVarList(input);

	while (input.good())
		{
		JBoolean expectedOK;
		input >> expectedOK;

		JComplex expectedResult;
		input >> expectedResult;

		JFunction* theFunction = NULL;
		if (!GetFunction(input, &theVarList, &theFunction))
			{
			break;
			}
		else if (theFunction != NULL)
			{
			theFunction->Print(std::cout);
			std::cout << std::endl;

			JComplex result;
			const JBoolean ok = theFunction->Evaluate(&result);
			std::cout << "Expected: " << expectedOK << ' ' << expectedResult << std::endl;
			std::cout << "Result:   " << ok << ' ' << result << std::endl;
			JWaitForReturn();

			jdelete theFunction;
			}

		input >> std::ws;
		}
}
