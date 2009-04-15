/******************************************************************************
 JXFixLenPGDirector.h

	Interface for the JXFixLenPGDirector class

	Copyright © 1996 by Glenn W. Bach. All rights reserved.
	Copyright © 1997 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXFixLenPGDirector
#define _H_JXFixLenPGDirector

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXPGDirectorBase.h>

class JXProgressDisplay;

class JXFixLenPGDirector : public JXPGDirectorBase
{
public:

	JXFixLenPGDirector(JXDirector* supervisor, JXProgressDisplay* pg,
					   const JCharacter* message, const JBoolean allowCancel);

	virtual ~JXFixLenPGDirector();

private:

// begin JXLayout


// end JXLayout

private:

	void	BuildWindow(JXProgressDisplay* pg,
						const JCharacter* message, const JBoolean allowCancel);

	// not allowed

	JXFixLenPGDirector(const JXFixLenPGDirector& source);
	const JXFixLenPGDirector& operator=(const JXFixLenPGDirector& source);
};

#endif
