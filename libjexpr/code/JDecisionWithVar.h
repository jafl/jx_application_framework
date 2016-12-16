/******************************************************************************
 JDecisionWithVar.h

	Interface for JDecisionWithVar class.

	Copyright (C) 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JDecisionWithVar
#define _H_JDecisionWithVar

#include <JDecision.h>

class JFunction;

class JDecisionWithVar : public JDecision
{
public:

	JDecisionWithVar(const JVariableList* theVariableList,
					 const JIndex variableIndex, JFunction* arrayIndex,
					 const JDecisionType type);
	JDecisionWithVar(const JDecisionWithVar& source);

	virtual ~JDecisionWithVar();

	virtual JBoolean	SameAs(const JDecision& theDecision) const;
	virtual void		BuildNodeList(JExprNodeList* nodeList, const JIndex myNode);

	void	PrintVariable(std::ostream& output) const;

	const JVariableList*	GetVariableList() const;

	JIndex			GetVariableIndex() const;
	virtual void	SetVariableIndex(const JIndex variableIndex);

	const JFunction*	GetArrayIndex() const;
	void				SetArrayIndex(JFunction* theArrayIndex);
	JBoolean			EvaluateArrayIndex(JIndex* index) const;

	// called by JVariableList

	virtual JBoolean	UsesVariable(const JIndex variableIndex) const;
	virtual void		VariablesInserted(const JIndex firstIndex, const JSize count);
	virtual void		VariablesRemoved(const JIndex firstIndex, const JSize count);
	virtual void		VariableMoved(const JIndex origIndex, const JIndex newIndex);
	virtual void		VariablesSwapped(const JIndex index1, const JIndex index2);

	// provides safe downcasting

	virtual JDecisionWithVar*			CastToJDecisionWithVar();
	virtual const JDecisionWithVar*		CastToJDecisionWithVar() const;

private:

	const JVariableList*	itsVariableList;
	JIndex					itsVariableIndex;
	JFunction*				itsArrayIndex;

private:

	// not allowed

	const JDecisionWithVar& operator=(const JDecisionWithVar& source);
};

/******************************************************************************
 GetVariableList

 ******************************************************************************/

inline const JVariableList*
JDecisionWithVar::GetVariableList()
	const
{
	return itsVariableList;
}

/******************************************************************************
 GetVariableIndex

 ******************************************************************************/

inline JIndex
JDecisionWithVar::GetVariableIndex()
	const
{
	return itsVariableIndex;
}

/******************************************************************************
 GetArrayIndex

 ******************************************************************************/

inline const JFunction*
JDecisionWithVar::GetArrayIndex()
	const
{
	return itsArrayIndex;
}

#endif
