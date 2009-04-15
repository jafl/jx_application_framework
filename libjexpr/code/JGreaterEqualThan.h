/******************************************************************************
 JGreaterEqualThan.h

	Interface for JGreaterEqualThan class.

	Copyright © 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JGreaterEqualThan
#define _H_JGreaterEqualThan

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JFunctionComparison.h>

class JGreaterEqualThan : public JFunctionComparison
{
public:

	JGreaterEqualThan();
	JGreaterEqualThan(JFunction* arg1, JFunction* arg2);
	JGreaterEqualThan(const JGreaterEqualThan& source);

	virtual ~JGreaterEqualThan();

	virtual JBoolean	Evaluate() const;
	virtual JDecision*	Copy() const;
	virtual JBoolean	SameAs(const JDecision& theDecision) const;

private:

	// not allowed

	const JGreaterEqualThan& operator=(const JGreaterEqualThan& source);
};

#endif
