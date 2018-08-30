/******************************************************************************
 JVariableValue.h

	Interface for JVariableValue class.

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#ifndef _H_JVariableValue
#define _H_JVariableValue

#include <JFunctionWithVar.h>

class JVariableValue : public JFunctionWithVar
{
public:

	JVariableValue(const JVariableList* theVariableList, const JIndex variableIndex,
				   JFunction* arrayIndex);
	JVariableValue(const JVariableValue& source);

	virtual ~JVariableValue();

	virtual JBoolean	Evaluate(JFloat* result) const override;
	virtual JBoolean	Evaluate(JComplex* result) const override;
	virtual void		Print(std::ostream& output) const override;
	virtual JFunction*	Copy() const override;

	virtual void	SetVariableIndex(const JIndex variableIndex) override;

private:

	// not allowed

	const JVariableValue& operator=(const JVariableValue& source);
};

#endif
