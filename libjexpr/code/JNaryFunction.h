/******************************************************************************
 JNaryFunction.h

	Interface for JNaryFunction class.

	Copyright © 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JNaryFunction
#define _H_JNaryFunction

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JFunctionWithArgs.h>
#include <JPtrArray.h>

class JNaryFunction : public JFunctionWithArgs
{
public:

	JNaryFunction(const JFnNameIndex nameIndex, const JFunctionType type);
	JNaryFunction(const JNaryFunction& source);

	virtual ~JNaryFunction();

	virtual JBoolean	SameAs(const JFunction& theFunction) const;

	virtual JSize				GetArgCount() const;
	JBoolean					ArgIndexValid(const JIndex index) const;
	virtual const JFunction*	GetArg(const JIndex index) const;
	virtual JFunction*			GetArg(const JIndex index);
	virtual void				SetArg(const JIndex index, JFunction* arg);
	void						InsertArg(const JIndex index, JFunction* arg);
	void						PrependArg(JFunction* arg);
	void						AppendArg(JFunction* arg);
	void						DeleteArg(const JIndex index);
	JBoolean					DeleteArg(const JFunction* arg);
	JBoolean					FindArg(JFunction* arg, JIndex* index);
	void						MoveArgToIndex(const JIndex currentIndex,
											   const JIndex newIndex);

	// provides safe downcasting

	virtual JNaryFunction*			CastToJNaryFunction();
	virtual const JNaryFunction*	CastToJNaryFunction() const;

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
