/******************************************************************************
 JXAtMostOneCBGroup.h

	Interface for the JXAtMostOneCBGroup class

	Copyright (C) 2006 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXAtMostOneCBGroup
#define _H_JXAtMostOneCBGroup

#include "JXCheckboxGroup.h"

class JXCheckbox;

class JXAtMostOneCBGroup : public JXCheckboxGroup
{
public:

	JXAtMostOneCBGroup();
	JXAtMostOneCBGroup(const JPtrArray<JXCheckbox>& cbList);
	JXAtMostOneCBGroup(const JSize count, JXCheckbox* cb1, JXCheckbox* cb2, ...);

	~JXAtMostOneCBGroup() override;

protected:

	void	EnforceConstraints(const JIndex cbIndex) override;

private:

	bool	itsIgnoreChangeFlag;
};

#endif
