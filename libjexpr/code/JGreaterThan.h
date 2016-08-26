/******************************************************************************
 JGreaterThan.h

	Interface for JGreaterThan class.

	Copyright © 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JGreaterThan
#define _H_JGreaterThan

#include <JFunctionComparison.h>

class JGreaterThan : public JFunctionComparison
{
public:

	JGreaterThan();
	JGreaterThan(JFunction* arg1, JFunction* arg2);
	JGreaterThan(const JGreaterThan& source);

	virtual ~JGreaterThan();

	virtual JBoolean	Evaluate() const;
	virtual JDecision*	Copy() const;
	virtual JBoolean	SameAs(const JDecision& theDecision) const;

private:

	// not allowed

	const JGreaterThan& operator=(const JGreaterThan& source);
};

#endif
