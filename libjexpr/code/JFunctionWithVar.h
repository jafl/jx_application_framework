/******************************************************************************
 JFunctionWithVar.h

	Interface for JFunctionWithVar class.

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#ifndef _H_JFunctionWithVar
#define _H_JFunctionWithVar

#include "JFunction.h"
#include <jx-af/jcore/jNew.h>

class JFunctionWithVar : public JFunction
{
public:

	JFunctionWithVar(const JVariableList* theVariableList,
					 const JIndex variableIndex, JFunction* arrayIndex);
	JFunctionWithVar(const JFunctionWithVar& source);

	~JFunctionWithVar() override;

	JIndex	Layout(const JExprRenderer& renderer,
				   const JPoint& upperLeft, const JSize fontSize,
				   JExprRectList* rectList) override;
	void	Render(const JExprRenderer& renderer,
				   const JExprRectList& rectList) const override;

	void	PrintVariable(std::ostream& output) const;

	const JVariableList*	GetVariableList() const;

	JIndex			GetVariableIndex() const;
	virtual void	SetVariableIndex(const JIndex variableIndex);

	const JFunction*	GetArrayIndex() const;
	void				SetArrayIndex(JFunction* theArrayIndex);
	bool				EvaluateArrayIndex(JIndex* index) const;

	// called by JVariableList

	bool	UsesVariable(const JIndex variableIndex) const override;
	void	VariablesInserted(const JIndex firstIndex, const JSize count) override;
	void	VariablesRemoved(const JIndex firstIndex, const JSize count) override;
	void	VariableMoved(const JIndex origIndex, const JIndex newIndex) override;
	void	VariablesSwapped(const JIndex index1, const JIndex index2) override;

private:

	const JVariableList*	itsVariableList;
	JIndex					itsVariableIndex;
	JFunction*				itsArrayIndex;		// can be nullptr
};

/******************************************************************************
 GetVariableList

 ******************************************************************************/

inline const JVariableList*
JFunctionWithVar::GetVariableList()
	const
{
	return itsVariableList;
}

/******************************************************************************
 GetVariableIndex

 ******************************************************************************/

inline JIndex
JFunctionWithVar::GetVariableIndex()
	const
{
	return itsVariableIndex;
}

/******************************************************************************
 GetArrayIndex

 ******************************************************************************/

inline const JFunction*
JFunctionWithVar::GetArrayIndex()
	const
{
	return itsArrayIndex;
}

/******************************************************************************
 SetArrayIndex

 ******************************************************************************/

inline void
JFunctionWithVar::SetArrayIndex
	(
	JFunction* theArrayIndex
	)
{
	jdelete itsArrayIndex;
	itsArrayIndex = theArrayIndex;
}

#endif
