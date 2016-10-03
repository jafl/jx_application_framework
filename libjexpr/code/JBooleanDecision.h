/******************************************************************************
 JBooleanDecision.h

	Interface for JBooleanDecision class.

	Copyright (C) 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JBooleanDecision
#define _H_JBooleanDecision

#include <JBinaryDecision.h>

class JBooleanDecision : public JBinaryDecision<JDecision>
{
public:

	JBooleanDecision(const JCharacter* name, const JDecisionType type);
	JBooleanDecision(JDecision* arg1, JDecision* arg2,
					 const JCharacter* name, const JDecisionType type);
	JBooleanDecision(const JBooleanDecision& source);

	virtual ~JBooleanDecision();

	virtual void		Print(ostream& output) const;
	virtual JBoolean	SameAs(const JDecision& theDecision) const;
	virtual void		BuildNodeList(JExprNodeList* nodeList, const JIndex myNode);

	// provides safe downcasting

	virtual JBooleanDecision*		CastToJBooleanDecision();
	virtual const JBooleanDecision*	CastToJBooleanDecision() const;

protected:

	void	PrintArg(ostream& output, const JDecision* arg) const;

private:

	// not allowed

	const JBooleanDecision& operator=(const JBooleanDecision& source);
};

#endif
