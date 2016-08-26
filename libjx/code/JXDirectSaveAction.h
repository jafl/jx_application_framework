/******************************************************************************
 JXDirectSaveAction.h

	Copyright © 2001 by John Lindal. All rights reserved.

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

	JXDirectSaveAction(const JXDirectSaveAction& source);
	const JXDirectSaveAction& operator=(const JXDirectSaveAction& source);
};

#endif
