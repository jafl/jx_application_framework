/******************************************************************************
 test_JExprParser.cpp

	Tests for JExprParser.

	Written by John Lindal.

 *****************************************************************************/

#include <JTestManager.h>
#include <JExprParser.h>
#include "TestVarList.h"
#include "TestFontManager.h"
#include <JFunction.h>
#include <jStreamUtil.h>
#include <fstream>
#include <jAssert.h>

int main()
{
	return JTestManager::Execute();
}

JTEST(RealFunction)
{
	std::ifstream input("./data/test_real_function.txt");

	TestVarList varList(input);
	JExprParser p(&varList);

	JString expr, msg;
	while (1)
		{
		JBoolean expectedParseOK;
		input >> expectedParseOK;
		if (!input.good())
			{
			break;
			}

		JBoolean expectedEvalOK;
		input >> expectedEvalOK;

		JFloat expectedResult;
		input >> expectedResult;

		input >> std::ws;
		expr = JReadLine(input);

		JFunction* f;
		JBoolean ok = p.Parse(expr, &f);

		msg = "Parse failed: " + expr;
		JAssertEqualWithMessage(expectedParseOK, ok, msg.GetBytes());

		if (ok)
			{
			JFloat result;
			ok = f->Evaluate(&result);

			msg = "Eval failed: " + expr;
			JAssertEqualWithMessage(expectedEvalOK, ok, msg.GetBytes());

			if (ok)
				{
				msg = "Eval incorrect: " + expr;
				JAssertWithinWithMessage(1e-5, expectedResult, result, msg.GetBytes());
				}
			}

		jdelete f;
		}
}

JTEST(ComplexFunction)
{
	std::ifstream input("./data/test_complex_function.txt");

	TestVarList varList(input);
	JExprParser p(&varList);

	JString expr, msg;
	while (1)
		{
		JBoolean expectedParseOK;
		input >> expectedParseOK;
		if (!input.good())
			{
			break;
			}

		JBoolean expectedEvalOK;
		input >> expectedEvalOK;

		JComplex expectedResult;
		input >> expectedResult;

		input >> std::ws;
		expr = JReadLine(input);

		JFunction* f;
		JBoolean ok = p.Parse(expr, &f);

		msg = "Parse failed: " + expr;
		JAssertEqualWithMessage(expectedParseOK, ok, msg.GetBytes());

		if (ok)
			{
			JComplex result;
			ok = f->Evaluate(&result);

			msg = "Eval failed: " + expr;
			JAssertEqualWithMessage(expectedEvalOK, ok, msg.GetBytes());

			if (ok)
				{
				msg = "Eval real incorrect: " + expr;
				JAssertWithinWithMessage(1e-5, expectedResult.real(), result.real(), msg.GetBytes());

				msg = "Eval imag incorrect: " + expr;
				JAssertWithinWithMessage(1e-5, expectedResult.imag(), result.imag(), msg.GetBytes());
				}
			}

		jdelete f;
		}
}

JTEST(Printing)
{
	std::ifstream input("./data/test_print_function.txt");

	TestVarList varList(input);
	JExprParser p(&varList);

	JString expr, expected, result, msg;
	while (1)
		{
		input >> expr >> expected;
		if (!input.good())
			{
			break;
			}

		JFunction* f;
		JBoolean ok = p.Parse(expr, &f);
		JAssertTrue(ok);

		if (ok)
			{
			result = f->Print();

			msg = "Print incorrect: " + expr;
			JAssertEqualWithMessage(expected, result, msg.GetBytes());
			}

		jdelete f;
		}
}
