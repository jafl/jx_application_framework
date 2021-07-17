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
	bool					ArgIndexValid(const JIndex index) const;
	virtual const JFunction*	GetArg(const JIndex index) const override;
	virtual JFunction*			GetArg(const JIndex index) override;
	virtual void				SetArg(const JIndex index, JFunction* arg) override;
	void						InsertArg(const JIndex index, JFunction* arg);
	void						PrependArg(JFunction* arg);
	void						AppendArg(JFunction* arg);
	void						DeleteArg(const JIndex index);
	bool					DeleteArg(const JFunction* arg);
	bool					FindArg(JFunction* arg, JIndex* index);
	void						MoveArgToIndex(const JIndex currentIndex,
											   const JIndex newIndex);

protected:

	bool	SameAsSameOrder(const JFunction& theFunction) const;
	bool	SameAsAnyOrder(const JFunction& theFunction) const;

private:

	JPtrArray<JFunction>*	itsArgList;

private:

	// not allowed

	const JNaryFunction& operator=(const JNaryFunction& source);
};

/******************************************************************************
 FindArg

 ******************************************************************************/

inline bool
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

inline bool
JNaryFunction::ArgIndexValid
	(
	const JIndex index
	)
	const
{
	return itsArgList->IndexValid(index);
}

#endif
