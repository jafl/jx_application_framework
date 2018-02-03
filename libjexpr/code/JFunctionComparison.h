/******************************************************************************
 JFunctionComparison.h

	Interface for JFunctionComparison class.

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#ifndef _H_JFunctionComparison
#define _H_JFunctionComparison

#include <JBinaryDecision.h>
#include <JFunction.h>

class JFunctionComparison : public JBinaryDecision<JFunction>
{
public:

	JFunctionComparison(const JCharacter* name, const JDecisionType type);
	JFunctionComparison(JFunction* arg1, JFunction* arg2,
						const JCharacter* name, const JDecisionType type);
	JFunctionComparison(const JFunctionComparison& source);

	virtual ~JFunctionComparison();

	virtual void	BuildNodeList(JExprNodeList* nodeList, const JIndex myNode);

	// provides safe downcasting

	virtual JFunctionComparison*		CastToJFunctionComparison();
	virtual const JFunctionComparison*	CastToJFunctionComparison() const;

private:

	// not allowed

	const JFunctionComparison& operator=(const JFunctionComparison& source);
};

#endif
