/******************************************************************************
 JXAtLeastOneCBGroup.h

	Interface for the JXAtLeastOneCBGroup class

	Copyright © 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXAtLeastOneCBGroup
#define _H_JXAtLeastOneCBGroup

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXCheckboxGroup.h>

class JXCheckbox;

class JXAtLeastOneCBGroup : public JXCheckboxGroup
{
public:

	JXAtLeastOneCBGroup();
	JXAtLeastOneCBGroup(const JPtrArray<JXCheckbox>& cbList);
	JXAtLeastOneCBGroup(const JSize	count, JXCheckbox* cb1, JXCheckbox* cb2, ...);

	virtual ~JXAtLeastOneCBGroup();

protected:

	virtual void	EnforceConstraints(const JIndex cbIndex);

private:

	enum Direction
	{
		kSlideUp,
		kSlideDown
	};

private:

	Direction	itsDirection;
	JBoolean	itsIgnoreChangeFlag;

private:

	JIndex	GetNextActiveIndex(const JIndex cbIndex);

	// not allowed

	JXAtLeastOneCBGroup(const JXAtLeastOneCBGroup& source);
	const JXAtLeastOneCBGroup& operator=(const JXAtLeastOneCBGroup& source);
};

#endif
