/******************************************************************************
 JBooleanXOR.h

	Interface for JBooleanXOR class.

	Copyright © 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JBooleanXOR
#define _H_JBooleanXOR

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JBooleanDecision.h>

class JBooleanXOR : public JBooleanDecision
{
public:

	JBooleanXOR();
	JBooleanXOR(JDecision* arg1, JDecision* arg2);
	JBooleanXOR(const JBooleanXOR& source);

	virtual ~JBooleanXOR();

	virtual JBoolean	Evaluate() const;
	virtual JDecision*	Copy() const;

private:

	// not allowed

	const JBooleanXOR& operator=(const JBooleanXOR& source);
};

#endif
