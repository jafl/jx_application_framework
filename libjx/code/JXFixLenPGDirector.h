/******************************************************************************
 JXFixLenPGDirector.h

	Interface for the JXFixLenPGDirector class

	Copyright (C) 1996 by Glenn W. Bach.
	Copyright (C) 1997 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXFixLenPGDirector
#define _H_JXFixLenPGDirector

#include "jx-af/jx/JXPGDirectorBase.h"

class JXProgressDisplay;

class JXFixLenPGDirector : public JXPGDirectorBase
{
public:

	JXFixLenPGDirector(JXDirector* supervisor, JXProgressDisplay* pg,
					   const JString& message, const bool allowCancel);

	~JXFixLenPGDirector();

private:

// begin JXLayout


// end JXLayout

private:

	void	BuildWindow(JXProgressDisplay* pg,
						const JString& message, const bool allowCancel);
};

#endif
