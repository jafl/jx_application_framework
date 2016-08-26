/******************************************************************************
 JLessThan.h

	Interface for JLessThan class.

	Copyright © 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JLessThan
#define _H_JLessThan

#include <JFunctionComparison.h>

class JLessThan : public JFunctionComparison
{
public:

	JLessThan();
	JLessThan(JFunction* arg1, JFunction* arg2);
	JLessThan(const JLessThan& source);

	virtual ~JLessThan();

	virtual JBoolean	Evaluate() const;
	virtual JDecision*	Copy() const;
	virtual JBoolean	SameAs(const JDecision& theDecision) const;

private:

	// not allowed

	const JLessThan& operator=(const JLessThan& source);
};

#endif
