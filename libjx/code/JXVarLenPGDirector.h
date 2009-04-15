/******************************************************************************
 JXVarLenPGDirector.h

	Interface for the JXVarLenPGDirector class

	Copyright © 1996 by Glenn W. Bach. All rights reserved.
	Copyright © 1997 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXVarLenPGDirector
#define _H_JXVarLenPGDirector

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXPGDirectorBase.h>

class JXProgressDisplay;

class JXVarLenPGDirector : public JXPGDirectorBase
{
public:

	JXVarLenPGDirector(JXDirector* supervisor, JXProgressDisplay* pg,
					   const JCharacter* message, const JBoolean allowCancel);

	virtual ~JXVarLenPGDirector();

private:

// begin JXLayout


// end JXLayout

private:

	void	BuildWindow(JXProgressDisplay* pg, const JCharacter* message,
						const JBoolean allowCancel);

	// not allowed

	JXVarLenPGDirector(const JXVarLenPGDirector& source);
	const JXVarLenPGDirector& operator=(const JXVarLenPGDirector& source);
};

#endif
