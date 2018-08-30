/******************************************************************************
 JBooleanOR.h

	Interface for JBooleanOR class.

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#ifndef _H_JBooleanOR
#define _H_JBooleanOR

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
