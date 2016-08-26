/******************************************************************************
 JFunctionEquality.h

	Interface for JFunctionEquality class.

	Copyright © 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JFunctionEquality
#define _H_JFunctionEquality

#include <JFunctionComparison.h>

class JFunctionEquality : public JFunctionComparison
{
public:

	JFunctionEquality();
	JFunctionEquality(JFunction* arg1, JFunction* arg2);
	JFunctionEquality(const JFunctionEquality& source);

	virtual ~JFunctionEquality();

	virtual JBoolean	Evaluate() const;
	virtual JDecision*	Copy() const;
	virtual JBoolean	SameAs(const JDecision& theDecision) const;

private:

	// not allowed

	const JFunctionEquality& operator=(const JFunctionEquality& source);
};

#endif
