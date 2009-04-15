/******************************************************************************
 JXAssert.h

	Base class for reporting fatal errors

	Copyright © 1997 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXAssert
#define _H_JXAssert

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JAssertBase.h>
#include <JPtrArray.h>
#include <JXDisplay.h>	// for Windows template compile

class JXApplication;

class JXAssert : public JAssertBase
{
	friend class JXApplication;

public:

	JXAssert();

	virtual	~JXAssert();

	virtual int		Assert(const JCharacter* expr, const JCharacter* file,
						   const int line);
	virtual void	Abort();

	void	UnlockDisplays();

private:

	JBoolean				itsIsOperatingFlag;		// kJTrue if assert() is being processed
	JPtrArray<JXDisplay>*	itsDisplayList;

private:

	// called by JXApplication

	void	DisplayOpened(JXDisplay* display);
	void	DisplayClosed(JXDisplay* display);

	// not allowed

	JXAssert(const JXAssert& source);
	const JXAssert& operator=(const JXAssert& source);
};

/******************************************************************************
 Routines for JXApplication

 ******************************************************************************/

inline void
JXAssert::DisplayOpened
	(
	JXDisplay* display
	)
{
	itsDisplayList->Append(display);
}

inline void
JXAssert::DisplayClosed
	(
	JXDisplay* display
	)
{
	itsDisplayList->Remove(display);
}

#endif
