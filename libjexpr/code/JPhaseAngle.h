/******************************************************************************
 JPhaseAngle.h

	Interface for JPhaseAngle class.

	Copyright (C) 1997 by John Lindal.

 ******************************************************************************/

#ifndef _H_JPhaseAngle
#define _H_JPhaseAngle

#include <JUnaryFunction.h>

class JPhaseAngle : public JUnaryFunction
{
public:

	JPhaseAngle(JFunction* arg = nullptr);
	JPhaseAngle(const JPhaseAngle& source);

	virtual ~JPhaseAngle();

	virtual bool	Evaluate(JFloat* result) const override;
	virtual bool	Evaluate(JComplex* result) const override;
	virtual JFunction*	Copy() const override;

private:

	// not allowed

	const JPhaseAngle& operator=(const JPhaseAngle& source);
};

#endif
