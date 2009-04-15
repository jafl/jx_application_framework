/******************************************************************************
 JPiecewiseFunction.h

	Interface for JPiecewiseFunction class.

	Copyright © 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JPiecewiseFunction
#define _H_JPiecewiseFunction

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JPtrArray.h>

class JVariableList;
class JDecision;
class JFunction;

class JPiecewiseFunction
{
	friend ostream& operator<<(ostream& output, const JPiecewiseFunction& aPWFunc);

public:

	JPiecewiseFunction(JFunction* defaultFunction);
	JPiecewiseFunction(istream& input, JVariableList* theVariableList);
	JPiecewiseFunction(const JPiecewiseFunction& source);

	virtual ~JPiecewiseFunction();

	virtual JBoolean	Evaluate(JFloat* result) const;
	virtual JBoolean	SameAs(const JPiecewiseFunction& thePWFunction) const;

	void	AddCase(JDecision* theDecision, JFunction* theFunction);
	void	SetDefaultFunction(JFunction* theFunction);

private:

	JPtrArray<JDecision>*	itsCases;
	JPtrArray<JFunction>*	itsFunctions;
	JFunction*				itsDefaultFunction;

private:

	// not allowed

	const JPiecewiseFunction& operator=(const JPiecewiseFunction& source);
};

#endif
