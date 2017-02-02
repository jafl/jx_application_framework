/******************************************************************************
 JXFixLenPGDirector.h

	Interface for the JXFixLenPGDirector class

	Copyright (C) 1996 by Glenn W. Bach. All rights reserved.
	Copyright (C) 1997 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXFixLenPGDirector
#define _H_JXFixLenPGDirector

#include <JXPGDirectorBase.h>

class JXProgressDisplay;

class JXFixLenPGDirector : public JXPGDirectorBase
{
public:

	JXFixLenPGDirector(JXDirector* supervisor, JXProgressDisplay* pg,
					   const JString& message, const JBoolean allowCancel);

	virtual ~JXFixLenPGDirector();

private:

// begin JXLayout


// end JXLayout

private:

	void	BuildWindow(JXProgressDisplay* pg,
						const JString& message, const JBoolean allowCancel);

	// not allowed

	JXFixLenPGDirector(const JXFixLenPGDirector& source);
	const JXFixLenPGDirector& operator=(const JXFixLenPGDirector& source);
};

#endif
