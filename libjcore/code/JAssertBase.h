/******************************************************************************
 JAssertBase.h

	Base class for reporting fatal errors

	Copyright © 1997 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JAssertBase
#define _H_JAssertBase

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <jTypes.h>

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

	virtual int		Assert(const JCharacter* expr, const JCharacter* file,
						   const int line) = 0;
	virtual void	Abort();

	static int	DefaultAssert(const JCharacter* expr, const JCharacter* file,
							  const int line);

	static Action	GetAction();
	static void		SetAction(const Action action);

private:

	// not allowed

	JAssertBase(const JAssertBase& source);
	const JAssertBase& operator=(const JAssertBase& source);
};

#endif
