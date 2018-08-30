/******************************************************************************
 JNaryFunction.h

	Interface for JNaryFunction class.

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#ifndef _H_JNaryFunction
#define _H_JNaryFunction

#include <JFunctionWithArgs.h>
#include <JPtrArray.h>

class JNaryFunction : public JFunctionWithArgs
{
public:

	JNaryFunction(const JUtf8Byte* name, JPtrArray<JFunction>* argList = nullptr);
	JNaryFunction(const JNaryFunction& source);

	virtual ~JNaryFunction();

	virtual JSize				GetArgCount() const override;
	JBoolean					ArgIndexValid(const JIndex index) const;
	virtual const JFunction*	GetArg(const JIndex index) const override;
	virtual JFunction*			GetArg(const JIndex index) override;
	virtual void				SetArg(const JIndex index, JFunction* arg) override;
	void						InsertArg(const JIndex index, JFunction* arg);
	void						PrependArg(JFunction* arg);
	void						AppendArg(JFunction* arg);
	void						DeleteArg(const JIndex index);
	JBoolean					DeleteArg(const JFunction* arg);
	JBoolean					FindArg(JFunction* arg, JIndex* index);
	void						MoveArgToIndex(const JIndex currentIndex,
											   const JIndex newIndex);

protected:

	JBoolean	SameAsSameOrder(const JFunction& theFunction) const;
	JBoolean	SameAsAnyOrder(const JFunction& theFunction) const;

private:

	JPtrArray<JFunction>*	itsArgList;

private:

	// not allowed

	const JNaryFunction& operator=(const JNaryFunction& source);
};

/******************************************************************************
 FindArg

 ******************************************************************************/

inline JBoolean
JNaryFunction::FindArg
	(
	JFunction*	arg,
	JIndex*		index
	)
{
	return itsArgList->Find(arg, index);
}

/******************************************************************************
 MoveArgToIndex

 ******************************************************************************/

inline void
JNaryFunction::MoveArgToIndex
	(
	const JIndex currentIndex,
	const JIndex newIndex
	)
{
	itsArgList->MoveElementToIndex(currentIndex, newIndex);
}

/******************************************************************************
 ArgIndexValid

 ******************************************************************************/

inline JBoolean
JNaryFunction::ArgIndexValid
	(
	const JIndex index
	)
	const
{
	return itsArgList->IndexValid(index);
}

#endif
