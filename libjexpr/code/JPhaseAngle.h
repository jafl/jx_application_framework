/******************************************************************************
 JPhaseAngle.h

	Interface for JPhaseAngle class.

	Copyright (C) 1997 by John Lindal.

 ******************************************************************************/

#ifndef _H_JPhaseAngle
#define _H_JPhaseAngle

#include "JUnaryFunction.h"

class JPhaseAngle : public JUnaryFunction
{
public:

	JPhaseAngle(JFunction* arg = nullptr);
	JPhaseAngle(const JPhaseAngle& source);

	~JPhaseAngle() override;

	bool		Evaluate(JFloat* result) const override;
	bool		Evaluate(JComplex* result) const override;
	JFunction*	Copy() const override;
};

#endif
