/******************************************************************************
 JFunctionComparison.cpp

								The JFunctionComparison Class

	This class provides routines for all 2 argument JFunction comparisons.

	BASE CLASS = JBinaryDecision<JFunction>

	Copyright © 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JFunctionComparison.h>
#include <JExprNodeList.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JFunctionComparison::JFunctionComparison
	(
	const JCharacter*	name,
	const JDecisionType	type
	)
	:
	JBinaryDecision<JFunction>(name, type)
{
}

JFunctionComparison::JFunctionComparison
	(
	JFunction*			arg1,
	JFunction*			arg2,
	const JCharacter*	name,
	const JDecisionType	type
	)
	:
	JBinaryDecision<JFunction>(arg1, arg2, name, type)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JFunctionComparison::~JFunctionComparison()
{
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JFunctionComparison::JFunctionComparison
	(
	const JFunctionComparison& source
	)
	:
	JBinaryDecision<JFunction>(source)
{
}

/******************************************************************************
 BuildNodeList

 ******************************************************************************/

void
JFunctionComparison::BuildNodeList
	(
	JExprNodeList*	nodeList,
	const JIndex	myNode
	)
{
	nodeList->RecurseNodesForFunction(myNode, GetArg1());
	nodeList->RecurseNodesForFunction(myNode, GetArg2());
}

/******************************************************************************
 Cast to JFunctionComparison*

	Not inline because they are virtual

 ******************************************************************************/

JFunctionComparison*
JFunctionComparison::CastToJFunctionComparison()
{
	return this;
}

const JFunctionComparison*
JFunctionComparison::CastToJFunctionComparison()
	const
{
	return this;
}
