/******************************************************************************
 JAssertBase.h

	Base class for reporting fatal errors

	Copyright (C) 1997 by John Lindal.

 ******************************************************************************/

#ifndef _H_JAssertBase
#define _H_JAssertBase

#include "jx-af/jcore/jTypes.h"

class JAssertBase
{
public:

	enum Action
	{
		kIgnoreFailure,
		kAbort,
		kAskUser
	};

public:

	JAssertBase();

	virtual	~JAssertBase();

	virtual int		Assert(const JUtf8Byte* expr, const JUtf8Byte* file,
						   const int line, const JUtf8Byte* message) = 0;
	virtual void	Abort();

	static int	DefaultAssert(const JUtf8Byte* expr, const JUtf8Byte* file,
							  const int line, const JUtf8Byte* message);

	static Action	GetAction();
	static void		SetAction(const Action action);

private:

	// not allowed

	JAssertBase(const JAssertBase&) = delete;
	JAssertBase& operator=(const JAssertBase&) = delete;
};

#endif
