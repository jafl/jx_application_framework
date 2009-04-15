/******************************************************************************
 JArcTangent2.h

	Interface for JArcTangent2 class.

	Copyright © 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JArcTangent2
#define _H_JArcTangent2

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JBinaryFunction.h>

class JArcTangent2 : public JBinaryFunction
{
public:

	JArcTangent2();
	JArcTangent2(JFunction* arg1, JFunction* arg2);
	JArcTangent2(const JArcTangent2& source);

	virtual ~JArcTangent2();

	virtual JBoolean	Evaluate(JFloat* result) const;
	virtual JBoolean	Evaluate(JComplex* result) const;
	virtual JFunction*	Copy() const;
	virtual JBoolean	SameAs(const JFunction& theFunction) const;

private:

	// not allowed

	const JArcTangent2& operator=(const JArcTangent2& source);
};

#endif
