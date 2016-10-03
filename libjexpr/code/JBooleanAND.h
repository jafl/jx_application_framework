/******************************************************************************
 JBooleanAND.h

	Interface for JBooleanAND class.

	Copyright (C) 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JBooleanAND
#define _H_JBooleanAND

#include <JBooleanDecision.h>

class JBooleanAND : public JBooleanDecision
{
public:

	JBooleanAND();
	JBooleanAND(JDecision* arg1, JDecision* arg2);
	JBooleanAND(const JBooleanAND& source);

	virtual ~JBooleanAND();

	virtual JBoolean	Evaluate() const;
	virtual JDecision*	Copy() const;

private:

	// not allowed

	const JBooleanAND& operator=(const JBooleanAND& source);
};

#endif
