/******************************************************************************
 test.expr.fns.h

	Copyright (C) 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_test_expr_fns
#define _H_test_expr_fns

#include <jTypes.h>

class JFunction;
class JDecision;
class JVariableList;
class TestVarList;

void TestDecisionEquality(const JCharacter* fileName);
void TestFunctionEquality(const JCharacter* fileName);

JBoolean GetDecision(istream& input, const JVariableList* theVariableList,
					 JDecision** theDecision);
JBoolean GetFunction(istream& input, const JVariableList* theVariableList,
					 JFunction** theFunction);

#endif
