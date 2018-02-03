/******************************************************************************
 JDiscreteVarEquality.h

	Interface for JDiscreteVarEquality class.

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#ifndef _H_JDiscreteVarEquality
#define _H_JDiscreteVarEquality

#include <JDecision.h>

class JFunction;

class JDiscreteVarEquality : public JDecision
{
public:

	JDiscreteVarEquality(const JVariableList* theVariableList,
						 const JIndex varIndex1, JFunction* arrayIndex1,
						 const JIndex varIndex2, JFunction* arrayIndex2);
	JDiscreteVarEquality(const JDiscreteVarEquality& source);

	virtual ~JDiscreteVarEquality();

	virtual JBoolean	Evaluate() const;
	virtual void		Print(std::ostream& output) const;
	virtual JDecision*	Copy() const;
	virtual JBoolean	SameAs(const JDecision& theDecision) const;
	virtual void		BuildNodeList(JExprNodeList* nodeList, const JIndex myNode);

	JIndex	GetVariableIndex(const JIndex index) const;
	void	SetVariableIndex(const JIndex index, const JIndex varIndex);

	const JFunction*	GetArrayIndex(const JIndex index) const;
	void				SetArrayIndex(const JIndex index, JFunction* theArrayIndex);

	// called by JVariableList

	virtual JBoolean	UsesVariable(const JIndex variableIndex) const;
	virtual void		VariablesInserted(const JIndex firstIndex, const JSize count);
	virtual void		VariablesRemoved(const JIndex firstIndex, const JSize count);
	virtual void		VariableMoved(const JIndex origIndex, const JIndex newIndex);
	virtual void		VariablesSwapped(const JIndex index1, const JIndex index2);

protected:

	JBoolean	VarIndexValid(const JIndex index) const;

private:

	const JVariableList*	itsVariableList;
	JIndex					itsVarIndex[2];
	JFunction*				itsArrayIndex[2];

private:

	// not allowed

	const JDiscreteVarEquality& operator=(const JDiscreteVarEquality& source);
};

/******************************************************************************
 VarIndexValid (protected)

 ******************************************************************************/

inline JBoolean
JDiscreteVarEquality::VarIndexValid
	(
	const JIndex index
	)
	const
{
	return JConvertToBoolean( index == 1 || index == 2 );
}

#endif
