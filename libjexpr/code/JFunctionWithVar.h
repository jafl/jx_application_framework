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
					 const JIndex variableIndex, JFunction* arrayIndex,
					 const JFunctionType type);
	JFunctionWithVar(const JFunctionWithVar& source);

	virtual ~JFunctionWithVar();

	virtual JIndex		PrepareToRender(const JExprRenderer& renderer,
										const JPoint& upperLeft, const JSize fontSize,
										JExprRectList* rectList);
	virtual void		Render(const JExprRenderer& renderer,
							   const JExprRectList& rectList) const;
	virtual JBoolean	SameAs(const JFunction& theFunction) const;
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

	virtual JFunctionWithVar*		CastToJFunctionWithVar();
	virtual const JFunctionWithVar*	CastToJFunctionWithVar() const;

private:

	const JVariableList*	itsVariableList;
	JIndex					itsVariableIndex;
	JFunction*				itsArrayIndex;

private:

	JSize	GetStringWidth(const JExprRenderer& renderer,
						   const JSize fontSize, const JString& str) const;
	void	DrawString(const JExprRenderer& renderer,
					   const JCoordinate left, const JCoordinate midline,
					   const JSize fontSize, const JString& str) const;

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
