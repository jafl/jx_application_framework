/******************************************************************************
 JDiscVarValue.h

	Interface for JDiscVarValue class.

	Copyright (C) 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JDiscVarValue
#define _H_JDiscVarValue

#include <JFunctionWithVar.h>

class JDiscVarValue : public JFunctionWithVar
{
public:

	JDiscVarValue(const JVariableList* theVariableList,
				  const JIndex variableIndex, JFunction* arrayIndex);
	JDiscVarValue(const JDiscVarValue& source);

	virtual ~JDiscVarValue();

	virtual JBoolean	Evaluate(JFloat* result) const;
	virtual JBoolean	Evaluate(JComplex* result) const;
	virtual void		Print(ostream& output) const;
	virtual JFunction*	Copy() const;

	virtual void	SetVariableIndex(const JIndex variableIndex);

private:

	// not allowed

	const JDiscVarValue& operator=(const JDiscVarValue& source);
};

#endif
