/******************************************************************************
 JNaryFunction.h

	Interface for JNaryFunction class.

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#ifndef _H_JNaryFunction
#define _H_JNaryFunction

#include "jx-af/jexpr/JFunctionWithArgs.h"
#include <jx-af/jcore/JPtrArray.h>

class JNaryFunction : public JFunctionWithArgs
{
public:

	JNaryFunction(const JUtf8Byte* name, JPtrArray<JFunction>* argList = nullptr);
	JNaryFunction(const JNaryFunction& source);

	virtual ~JNaryFunction();

	JSize				GetArgCount() const override;
	bool				ArgIndexValid(const JIndex index) const;
	const JFunction*	GetArg(const JIndex index) const override;
	JFunction*			GetArg(const JIndex index) override;
	void				SetArg(const JIndex index, JFunction* arg) override;
	void				InsertArg(const JIndex index, JFunction* arg);
	void				PrependArg(JFunction* arg);
	void				AppendArg(JFunction* arg);
	void				DeleteArg(const JIndex index);
	bool				DeleteArg(const JFunction* arg);
	bool				FindArg(JFunction* arg, JIndex* index) const;
	void				MoveArgToIndex(const JIndex currentIndex,
									   const JIndex newIndex);

protected:

	bool	SameAsSameOrder(const JFunction& theFunction) const;
	bool	SameAsAnyOrder(const JFunction& theFunction) const;

private:

	JPtrArray<JFunction>*	itsArgList;
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
	const
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
