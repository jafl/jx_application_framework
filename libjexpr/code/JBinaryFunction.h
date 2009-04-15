/******************************************************************************
 JBinaryFunction.h

	Interface for JBinaryFunction class.

	Copyright © 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JBinaryFunction
#define _H_JBinaryFunction

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JFunctionWithArgs.h>

class JBinaryFunction : public JFunctionWithArgs
{
public:

	JBinaryFunction(const JFnNameIndex nameIndex, const JFunctionType type);
	JBinaryFunction(JFunction* arg1, JFunction* arg2, const JFnNameIndex nameIndex,
					const JFunctionType type);
	JBinaryFunction(const JBinaryFunction& source);

	virtual ~JBinaryFunction();

	virtual JBoolean	SameAs(const JFunction& theFunction) const;

	virtual JSize				GetArgCount() const;
	virtual const JFunction*	GetArg(const JIndex index) const;
	virtual JFunction*			GetArg(const JIndex index);
	virtual void				SetArg(const JIndex index, JFunction* arg);

	const JFunction*	GetArg1() const;
	const JFunction*	GetArg2() const;
	JFunction*			GetArg1();
	JFunction*			GetArg2();
	void				SetArg1(JFunction* arg);
	void				SetArg2(JFunction* arg);

	// provides safe downcasting

	virtual JBinaryFunction*		CastToJBinaryFunction();
	virtual const JBinaryFunction*	CastToJBinaryFunction() const;

protected:

	JBoolean	SameAsSameOrder(const JFunction& theFunction) const;
	JBoolean	SameAsEitherOrder(const JFunction& theFunction) const;

private:

	JFunction*	itsArg1;
	JFunction*	itsArg2;

private:

	// not allowed

	const JBinaryFunction& operator=(const JBinaryFunction& source);
};

/******************************************************************************
 GetArg

 ******************************************************************************/

inline const JFunction*
JBinaryFunction::GetArg1()
	const
{
	return itsArg1;
}

inline const JFunction*
JBinaryFunction::GetArg2()
	const
{
	return itsArg2;
}

inline JFunction*
JBinaryFunction::GetArg1()
{
	return itsArg1;
}

inline JFunction*
JBinaryFunction::GetArg2()
{
	return itsArg2;
}

/******************************************************************************
 SetArg

 ******************************************************************************/

inline void
JBinaryFunction::SetArg1
	(
	JFunction* arg
	)
{
	delete itsArg1;
	itsArg1 = arg;
}

inline void
JBinaryFunction::SetArg2
	(
	JFunction* arg
	)
{
	delete itsArg2;
	itsArg2 = arg;
}

#endif
