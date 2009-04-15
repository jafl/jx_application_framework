/******************************************************************************
 JBinaryOperator.h

	Interface for JBinaryOperator class.

	Copyright © 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JBinaryOperator
#define _H_JBinaryOperator

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JBinaryFunction.h>

class JBinaryOperator : public JBinaryFunction
{
public:

	JBinaryOperator(const JFnNameIndex nameIndex, const JFunctionType type);
	JBinaryOperator(JFunction* arg1, JFunction* arg2, const JFnNameIndex nameIndex,
					const JFunctionType type);
	JBinaryOperator(const JBinaryOperator& source);

	virtual ~JBinaryOperator();

	virtual void	Print(ostream& output) const;

	// provides safe downcasting

	virtual JBinaryOperator*		CastToJBinaryOperator();
	virtual const JBinaryOperator*	CastToJBinaryOperator() const;

protected:

	void	PrintArg(ostream& output, const JFunction* arg) const;

private:

	// not allowed

	const JBinaryOperator& operator=(const JBinaryOperator& source);
};

#endif
