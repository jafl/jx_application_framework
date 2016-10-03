/******************************************************************************
 JXVarLenPGDirector.h

	Interface for the JXVarLenPGDirector class

	Copyright (C) 1996 by Glenn W. Bach. All rights reserved.
	Copyright (C) 1997 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXVarLenPGDirector
#define _H_JXVarLenPGDirector

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
