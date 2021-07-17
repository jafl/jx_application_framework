/******************************************************************************
 JFunctionWithVar.h

	Interface for JFunctionWithVar class.

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#ifndef _H_JFunctionWithVar
#define _H_JFunctionWithVar

#include <JFunction.h>
#include <jNew.h>

class JFunctionWithVar : public JFunction
{
public:

	JFunctionWithVar(const JVariableList* theVariableList,
					 const JIndex variableIndex, JFunction* arrayIndex);
	JFunctionWithVar(const JFunctionWithVar& source);

	virtual ~JFunctionWithVar();

	virtual JIndex		Layout(const JExprRenderer& renderer,
							   const JPoint& upperLeft, const JSize fontSize,
							   JExprRectList* rectList) override;
	virtual void		Render(const JExprRenderer& renderer,
							   const JExprRectList& rectList) const override;

	void	PrintVariable(std::ostream& output) const;

	const JVariableList*	GetVariableList() const;

	JIndex			GetVariableIndex() const;
	virtual void	SetVariableIndex(const JIndex variableIndex);

	const JFunction*	GetArrayIndex() const;
	void				SetArrayIndex(JFunction* theArrayIndex);
	bool			EvaluateArrayIndex(JIndex* index) const;

	// called by JVariableList

	virtual bool	UsesVariable(const JIndex variableIndex) const override;
	virtual void		VariablesInserted(const JIndex firstIndex, const JSize count) override;
	virtual void		VariablesRemoved(const JIndex firstIndex, const JSize count) override;
	virtual void		VariableMoved(const JIndex origIndex, const JIndex newIndex) override;
	virtual void		VariablesSwapped(const JIndex index1, const JIndex index2) override;

private:

	const JVariableList*	itsVariableList;
	JIndex					itsVariableIndex;
	JFunction*				itsArrayIndex;		// can be nullptr

private:

	// not allowed

	const JFunctionWithVar& operator=(const JFunctionWithVar& source);
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
