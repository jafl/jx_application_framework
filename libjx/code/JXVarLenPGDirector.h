/******************************************************************************
 JXVarLenPGDirector.h

	Interface for the JXVarLenPGDirector class

	Copyright (C) 1996 by Glenn W. Bach.
	Copyright (C) 1997 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXVarLenPGDirector
#define _H_JXVarLenPGDirector

#include "jx-af/jx/JXPGDirectorBase.h"

class JXProgressDisplay;

class JXVarLenPGDirector : public JXPGDirectorBase
{
public:

	JXVarLenPGDirector(JXDirector* supervisor, JXProgressDisplay* pg,
					   const JString& message, const bool allowCancel);

	~JXVarLenPGDirector();

private:

// begin JXLayout


// end JXLayout

private:

	void	BuildWindow(JXProgressDisplay* pg, const JString& message,
						const bool allowCancel);
};

#endif
