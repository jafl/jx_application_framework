/******************************************************************************
 JXAssert.h

	Base class for reporting fatal errors

	Copyright (C) 1997 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXAssert
#define _H_JXAssert

#include <jx-af/jcore/JAssertBase.h>
#include <jx-af/jcore/JPtrArray.h>
#include "JXDisplay.h"	// for Windows template compile

class JXApplication;

class JXAssert : public JAssertBase
{
	friend class JXApplication;

public:

	JXAssert();

	~JXAssert() override;

	int		Assert(const JUtf8Byte* expr, const JUtf8Byte* file,
				   const int line, const JUtf8Byte* message,
				   const JUtf8Byte* function) override;
	void	Abort() override;

	void	UnlockDisplays();

private:

	bool					itsIsOperatingFlag;		// true if assert() is being processed
	JPtrArray<JXDisplay>*	itsDisplayList;

private:

	// called by JXApplication

	void	DisplayOpened(JXDisplay* display);
	void	DisplayClosed(JXDisplay* display);
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
