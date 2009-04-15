/******************************************************************************
 JHypSine.h

	Interface for JHypSine class.

	Copyright © 1997 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JHypSine
#define _H_JHypSine

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JUnaryFunction.h>

class JHypSine : public JUnaryFunction
{
public:

	JHypSine();
	JHypSine(JFunction* arg);
	JHypSine(const JHypSine& source);

	virtual ~JHypSine();

	virtual JBoolean	Evaluate(JFloat* result) const;
	virtual JBoolean	Evaluate(JComplex* result) const;
	virtual JFunction*	Copy() const;

private:

	// not allowed

	const JHypSine& operator=(const JHypSine& source);
};

#endif
