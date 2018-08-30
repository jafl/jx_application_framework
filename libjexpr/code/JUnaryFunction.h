/******************************************************************************
 JUnaryFunction.h

	Interface for JUnaryFunction class.

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#ifndef _H_JUnaryFunction
#define _H_JUnaryFunction

#include <JFunctionWithArgs.h>
#include <jNew.h>

class JUnaryFunction : public JFunctionWithArgs
{
public:

	JUnaryFunction(const JUtf8Byte* name, JFunction* arg = nullptr);
	JUnaryFunction(const JUnaryFunction& source);

	virtual ~JUnaryFunction();

	virtual JSize				GetArgCount() const override;
	virtual const JFunction*	GetArg(const JIndex index) const override;
	virtual JFunction*			GetArg(const JIndex index) override;
	virtual void				SetArg(const JIndex index, JFunction* arg) override;

	const JFunction*	GetArg() const;
	JFunction*			GetArg();
	void				SetArg(JFunction* arg);

private:

	JFunction*	itsArg;

private:

	// not allowed

	const JUnaryFunction& operator=(const JUnaryFunction& source);
};

/******************************************************************************
 GetArg

 ******************************************************************************/

inline const JFunction*
JUnaryFunction::GetArg()
	const
{
	return itsArg;
}

inline JFunction*
JUnaryFunction::GetArg()
{
	return itsArg;
}

/******************************************************************************
 SetArg

 ******************************************************************************/

inline void
JUnaryFunction::SetArg
	(
	JFunction* arg
	)
{
	jdelete itsArg;
	itsArg = arg;
}

#endif
