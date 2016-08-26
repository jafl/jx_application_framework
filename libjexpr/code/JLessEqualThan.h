/******************************************************************************
 JLessEqualThan.h

	Interface for JLessEqualThan class.

	Copyright © 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JLessEqualThan
#define _H_JLessEqualThan

#include <JFunctionComparison.h>

class JLessEqualThan : public JFunctionComparison
{
public:

	JLessEqualThan();
	JLessEqualThan(JFunction* arg1, JFunction* arg2);
	JLessEqualThan(const JLessEqualThan& source);

	virtual ~JLessEqualThan();

	virtual JBoolean	Evaluate() const;
	virtual JDecision*	Copy() const;
	virtual JBoolean	SameAs(const JDecision& theDecision) const;

private:

	// not allowed

	const JLessEqualThan& operator=(const JLessEqualThan& source);
};

#endif
