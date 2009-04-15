/******************************************************************************
 JBooleanOR.h

	Interface for JBooleanOR class.

	Copyright © 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JBooleanOR
#define _H_JBooleanOR

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JBooleanDecision.h>

class JBooleanOR : public JBooleanDecision
{
public:

	JBooleanOR();
	JBooleanOR(JDecision* arg1, JDecision* arg2);
	JBooleanOR(const JBooleanOR& source);

	virtual ~JBooleanOR();

	virtual JBoolean	Evaluate() const;
	virtual JDecision*	Copy() const;

private:

	// not allowed

	const JBooleanOR& operator=(const JBooleanOR& source);
};

#endif
