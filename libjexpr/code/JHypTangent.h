/******************************************************************************
 JHypTangent.h

	Interface for JHypTangent class.

	Copyright © 1997 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JHypTangent
#define _H_JHypTangent

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JUnaryFunction.h>

class JHypTangent : public JUnaryFunction
{
public:

	JHypTangent();
	JHypTangent(JFunction* arg);
	JHypTangent(const JHypTangent& source);

	virtual ~JHypTangent();

	virtual JBoolean	Evaluate(JFloat* result) const;
	virtual JBoolean	Evaluate(JComplex* result) const;
	virtual JFunction*	Copy() const;

private:

	// not allowed

	const JHypTangent& operator=(const JHypTangent& source);
};

#endif
