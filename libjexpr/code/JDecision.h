/******************************************************************************
 JDecision.h

	Interface for JDecision class.

	Copyright © 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_Decision
#define _H_Decision

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JDecisionType.h>

class JString;
class JVariableList;
class JExprNodeList;

class JBooleanDecision;
class JFunctionComparison;
class JDecisionWithVar;

class JDecision
{
public:

	JDecision(const JDecisionType type);
	JDecision(const JDecision& source);

	virtual ~JDecision();

	JDecisionType	GetType() const;

	virtual JBoolean	Evaluate() const = 0;
	JString				Print() const;
	virtual void		Print(ostream& output) const = 0;
	virtual JDecision*	Copy() const = 0;
	virtual JBoolean	SameAs(const JDecision& theDecision) const;
	virtual void		BuildNodeList(JExprNodeList* nodeList, const JIndex myNode) = 0;

	static JDecision*	StreamIn(istream& input, JVariableList* theVariableList);
	void				StreamOut(ostream& output) const;

	JDecision*	Copy(JVariableList* newVariableList) const;

	// called by JVariableList -- must call inherited

	virtual JBoolean	UsesVariable(const JIndex variableIndex) const;
	virtual void		VariablesInserted(const JIndex firstIndex, const JSize count);
	virtual void		VariablesRemoved(const JIndex firstIndex, const JSize count);
	virtual void		VariableMoved(const JIndex origIndex, const JIndex newIndex);
	virtual void		VariablesSwapped(const JIndex index1, const JIndex index2);

	// provides safe downcasting

	virtual JBooleanDecision*			CastToJBooleanDecision();
	virtual const JBooleanDecision*		CastToJBooleanDecision() const;
	virtual JFunctionComparison*		CastToJFunctionComparison();
	virtual const JFunctionComparison*	CastToJFunctionComparison() const;
	virtual JDecisionWithVar*			CastToJDecisionWithVar();
	virtual const JDecisionWithVar*		CastToJDecisionWithVar() const;

private:

	const JDecisionType	itsType;

private:

	// not allowed

	const JDecision& operator=(const JDecision& source);
};

// declarations of global functions for dealing with Decisions

int operator==(const JDecision& d1, const JDecision& d2);
int operator!=(const JDecision& d1, const JDecision& d2);

/******************************************************************************
 GetType

 ******************************************************************************/

inline JDecisionType
JDecision::GetType()
	const
{
	return itsType;
}

#endif
