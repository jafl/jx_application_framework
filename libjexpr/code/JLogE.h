/******************************************************************************
 JLogE.h

	Interface for JLogE class.

	Copyright © 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JLogE
#define _H_JLogE

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JUnaryFunction.h>

class JLogE : public JUnaryFunction
{
public:

	JLogE();
	JLogE(JFunction* arg);
	JLogE(const JLogE& source);

	virtual ~JLogE();

	virtual JBoolean	Evaluate(JFloat* result) const;
	virtual JBoolean	Evaluate(JComplex* result) const;
	virtual JFunction*	Copy() const;

private:

	// not allowed

	const JLogE& operator=(const JLogE& source);
};

#endif
