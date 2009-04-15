/******************************************************************************
 JHypCosine.h

	Interface for JHypCosine class.

	Copyright © 1997 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JHypCosine
#define _H_JHypCosine

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JUnaryFunction.h>

class JHypCosine : public JUnaryFunction
{
public:

	JHypCosine();
	JHypCosine(JFunction* arg);
	JHypCosine(const JHypCosine& source);

	virtual ~JHypCosine();

	virtual JBoolean	Evaluate(JFloat* result) const;
	virtual JBoolean	Evaluate(JComplex* result) const;
	virtual JFunction*	Copy() const;

private:

	// not allowed

	const JHypCosine& operator=(const JHypCosine& source);
};

#endif
