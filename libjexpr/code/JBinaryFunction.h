/******************************************************************************
 JBinaryFunction.h

	Interface for JBinaryFunction class.

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#ifndef _H_JBinaryFunction
#define _H_JBinaryFunction

#include "jx-af/jexpr/JFunctionWithArgs.h"
#include <jx-af/jcore/jNew.h>

class JBinaryFunction : public JFunctionWithArgs
{
public:

	JBinaryFunction(const JUtf8Byte* name, JFunction* arg1, JFunction* arg2);
	JBinaryFunction(const JBinaryFunction& source);

	~JBinaryFunction();

	JSize				GetArgCount() const override;
	const JFunction*	GetArg(const JIndex index) const override;
	JFunction*			GetArg(const JIndex index) override;
	void				SetArg(const JIndex index, JFunction* arg) override;

	const JFunction*	GetArg1() const;
	const JFunction*	GetArg2() const;
	JFunction*			GetArg1();
	JFunction*			GetArg2();
	void				SetArg1(JFunction* arg);
	void				SetArg2(JFunction* arg);

private:

	JFunction*	itsArg1;
	JFunction*	itsArg2;
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
	jdelete itsArg1;
	itsArg1 = arg;
	itsArg1->SetParent(this);
}

inline void
JBinaryFunction::SetArg2
	(
	JFunction* arg
	)
{
	jdelete itsArg2;
	itsArg2 = arg;
	itsArg2->SetParent(this);
}

#endif
