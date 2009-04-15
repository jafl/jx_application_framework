/******************************************************************************
 JXAtMostOneCBGroup.h

	Interface for the JXAtMostOneCBGroup class

	Copyright © 2006 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXAtMostOneCBGroup
#define _H_JXAtMostOneCBGroup

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXCheckboxGroup.h>

class JXCheckbox;

class JXAtMostOneCBGroup : public JXCheckboxGroup
{
public:

	JXAtMostOneCBGroup();
	JXAtMostOneCBGroup(const JPtrArray<JXCheckbox>& cbList);
	JXAtMostOneCBGroup(const JSize	count, JXCheckbox* cb1, JXCheckbox* cb2, ...);

	virtual ~JXAtMostOneCBGroup();

protected:

	virtual void	EnforceConstraints(const JIndex cbIndex);

private:

	JBoolean	itsIgnoreChangeFlag;

private:

	// not allowed

	JXAtMostOneCBGroup(const JXAtMostOneCBGroup& source);
	const JXAtMostOneCBGroup& operator=(const JXAtMostOneCBGroup& source);
};

#endif
