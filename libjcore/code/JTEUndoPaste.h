/******************************************************************************
 JTEUndoPaste.h

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JTEUndoPaste
#define _H_JTEUndoPaste

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JTEUndoTextBase.h>

class JTEUndoPaste : public JTEUndoTextBase
{
public:

	JTEUndoPaste(JTextEditor* te, const JSize pasteLength);

	virtual ~JTEUndoPaste();

	virtual void	SetPasteLength(const JSize length);
	virtual void	Undo();

protected:

	void	SetCurrentLength(const JSize length);

private:

	JIndex	itsOrigSelStart;
	JSize	itsLength;

private:

	// not allowed

	JTEUndoPaste(const JTEUndoPaste& source);
	const JTEUndoPaste& operator=(const JTEUndoPaste& source);
};


/******************************************************************************
 SetCurrentLength (protected)

 ******************************************************************************/

inline void
JTEUndoPaste::SetCurrentLength
	(
	const JSize length
	)
{
	itsLength = length;
}

#endif
