/******************************************************************************
 JXDirectSaveAction.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXDirectSaveAction
#define _H_JXDirectSaveAction

#include <JString.h>

class JXDirectSaveAction
{
public:

	JXDirectSaveAction();

	virtual ~JXDirectSaveAction();

	virtual void	Save(const JString& fullName) = 0;

private:

	// not allowed

	JXDirectSaveAction(const JXDirectSaveAction&) = delete;
	JXDirectSaveAction& operator=(const JXDirectSaveAction&) = delete;
};

#endif
