/******************************************************************************
 JFunction.h

	Interface for JFunction class.

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#ifndef _H_JFunction
#define _H_JFunction

#include <JComplex.h>
#include <JRect.h>

class JString;
class JFontManager;

class JExprRenderer;
class JExprRectList;
class JVariableList;

class JFunction
{
public:

	JFunction();
	JFunction(const JFunction& source);

	virtual ~JFunction();

	JBoolean	GetParent(JFunction** parent);
	JBoolean	GetParent(const JFunction** parent) const;
	void		SetParent(JFunction* parent);

	virtual JBoolean	Evaluate(JFloat* result) const = 0;
	virtual JBoolean	Evaluate(JComplex* result) const = 0;
	JString				Print() const;
	virtual void		Print(std::ostream& output) const = 0;
	virtual JIndex		Layout(const JExprRenderer& renderer,
							   const JPoint& upperLeft, const JSize fontSize,
							   JExprRectList* rectList);
	virtual void		Render(const JExprRenderer& renderer,
							   const JExprRectList& rectList) const;
	virtual JFunction*	Copy() const = 0;

	// called by JVariableList -- must call inherited

	virtual JBoolean	UsesVariable(const JIndex variableIndex) const;
	virtual void		VariablesInserted(const JIndex firstIndex, const JSize count);
	virtual void		VariablesRemoved(const JIndex firstIndex, const JSize count);
	virtual void		VariableMoved(const JIndex origIndex, const JIndex newIndex);
	virtual void		VariablesSwapped(const JIndex index1, const JIndex index2);

private:

	JFunction*	itsParent;

private:

	// not allowed

	const JFunction& operator=(const JFunction& source);
};


/******************************************************************************
 Parent

 ******************************************************************************/

inline JBoolean
JFunction::GetParent
	(
	JFunction** parent
	)
{
	*parent = itsParent;
	return JI2B( *parent != nullptr );
}

inline JBoolean
JFunction::GetParent
	(
	const JFunction** parent
	)
	const
{
	*parent = itsParent;
	return JI2B( *parent != nullptr );
}

inline void
JFunction::SetParent
	(
	JFunction* parent
	)
{
	itsParent = parent;
}

#endif
