/******************************************************************************
 JVariableValue.h

	Interface for JVariableValue class.

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#ifndef _H_JVariableValue
#define _H_JVariableValue

#include "JFunctionWithVar.h"

class JVariableValue : public JFunctionWithVar
{
public:

	JVariableValue(const JVariableList* theVariableList, const JIndex variableIndex,
				   JFunction* arrayIndex);
	JVariableValue(const JVariableValue& source);

	~JVariableValue() override;

	bool		Evaluate(JFloat* result) const override;
	bool		Evaluate(JComplex* result) const override;
	void		Print(std::ostream& output) const override;
	JFunction*	Copy() const override;
};

#endif
