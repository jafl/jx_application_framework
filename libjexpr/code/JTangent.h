/******************************************************************************
 JTangent.h

	Interface for JTangent class.

	Copyright © 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JTangent
#define _H_JTangent

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JUnaryFunction.h>

class JTangent : public JUnaryFunction
{
public:

	JTangent();
	JTangent(JFunction* arg);
	JTangent(const JTangent& source);

	virtual ~JTangent();

	virtual JBoolean	Evaluate(JFloat* result) const;
	virtual JBoolean	Evaluate(JComplex* result) const;
	virtual JFunction*	Copy() const;

private:

	// not allowed

	const JTangent& operator=(const JTangent& source);
};

#endif
