/******************************************************************************
 JXAtLeastOneCBGroup.h

	Interface for the JXAtLeastOneCBGroup class

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXAtLeastOneCBGroup
#define _H_JXAtLeastOneCBGroup

#include "jx-af/jx/JXCheckboxGroup.h"

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
	bool	itsIgnoreChangeFlag;

private:

	JIndex	GetNextActiveIndex(const JIndex cbIndex);
};

#endif
