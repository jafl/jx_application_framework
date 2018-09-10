/******************************************************************************
 test_JExprParser.cpp

	Test JExprParser.

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

	TestFontManager fontMgr;
	TestVarList varList(input);
	JExprParser p(&varList, &fontMgr);

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

	TestFontManager fontMgr;
	TestVarList varList(input);
	JExprParser p(&varList, &fontMgr);

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
