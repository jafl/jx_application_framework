/******************************************************************************
 JBinaryDecision.h

	Interface for JBinaryDecision class.

	Copyright © 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JBinaryDecision
#define _H_JBinaryDecision

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JDecision.h>

template <class T>
class JBinaryDecision : public JDecision
{
public:

	JBinaryDecision(const JCharacter* name, const JDecisionType type);
	JBinaryDecision(T* arg1, T* arg2, const JCharacter* name, const JDecisionType type);
	JBinaryDecision(const JBinaryDecision& source);

	virtual ~JBinaryDecision();

	virtual void		Print(ostream& output) const;
	virtual JBoolean	SameAs(const JDecision& theDecision) const;

	const JCharacter*	GetName() const;

	const T*	GetArg1() const;
	const T*	GetArg2() const;
	T*			GetArg1();
	T*			GetArg2();
	void		SetArg1(T* arg);
	void		SetArg2(T* arg);

protected:

	JBoolean	SameAsSameOrder(const JDecision& theDecision) const;
	JBoolean	SameAsEitherOrder(const JDecision& theDecision) const;

private:

	const JCharacter* itsName;

	T*	itsArg1;
	T*	itsArg2;

private:

	// not allowed

	const JBinaryDecision& operator=(const JBinaryDecision& source);
};

#include <JBinaryDecision.tmpl>

#endif
